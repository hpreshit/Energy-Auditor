#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

#include "udelay.h"

#include "ncp.h"
#include "ncp_uart.h"
#include "SensorNodeObject.h"
#include "mytime.h"
#include "logger.h"

#define COMMAND_RESPONSE_DELAY		(700000)

static char uart_command[128] = {0};

int getCmdResponse(unsigned char* uart_reponse);

void NCPHardReset(){
	NCPInit();
}

void NCPInit()
{
	NCP_UARTInit();
//	printf("[NCP UART INIT DONE]\r\n");
	LOG_OK("NCP UART INIT\r\n");
	GPIO_PinModeSet(gpioPortA,1, gpioModePushPull, 0);
	GPIO_PinOutClear(gpioPortA, 1);
	DelayMS(10);
	GPIO_PinOutSet(gpioPortA, 1);
	uint8_t resp_buff[65] = {0};
	NCP_ReceiveBlocking(resp_buff, 4);
	if(resp_buff[0] == 0 && resp_buff[1] == '\n' && resp_buff[2] == '\b' && resp_buff[3] == '\b' ){
		LOG_OK("NCP Start Sequence 0nbb\r\n");
	}
	getCmdResponse(resp_buff);
	LOG_INFO("NCP Details: %s\r\n",resp_buff);
	DelayS(3);
//	set_machine_mode();

	//TODO: connect to which wifi?
	//1. ask if need to input wifi and password?
	//2. Yes - scanf wifi and password(echo off)
	//3. No - take the already stored values
	//4. Connect/Auto-connect?

	//TODO:
	//turn of leds on NCP for power saving
	//figure out the way to sleep NCP
}

uint8_t SetNCP_HeartBeatLED(uint8_t state)
{
	if(state > 1){
		LOG_ERROR("LET Invalid State\r\n");
		return 1;
	}

	size_t command_len;
	uint8_t resp_buff[32] = { 0 };

	sprintf((char*)uart_command, "gse 4 %u\r\n",state);
	command_len = strlen((char*)uart_command);
	DEFINE_NCPDATA(data,uart_command,command_len);
	NCP_SendData(&data);
	DelayMS(10);
	if(getCmdResponse(resp_buff) < 0){
		return 2;
	}

	return 0;
}

uint8_t GetNCP_HeartBeatLED()
{
	size_t command_len;
	uint8_t resp_buff[32] = { 0 };

	NCP_ReceiveStart();
	sprintf((char*)uart_command, "gge 4\r\n");
	command_len = strlen((char*)uart_command);
	DEFINE_NCPDATA(data,uart_command,command_len);
	NCP_SendData(&data);
	DelayMS(10);
	if(getCmdResponse(resp_buff) < 0){
		return 2;
	}
	int i = atoi(&resp_buff[0]);
	return (uint8_t)i;
}

uint8_t NCP_SetRemoteLED(char *ip, int port, uint8_t state)
{
	if(ip == NULL || state > 1){
		return 1;
	}

	uint8_t resp_buff[32] = { 0 };

	NCP_ReceiveStart();
	sprintf((char*)uart_command,"http_get http://%s/command/gse\%204\%20%d\r\n",ip,state);
	size_t data_len = strlen((char*)uart_command);
	DEFINE_NCPDATA(data,uart_command,data_len);
	NCP_SendData(&data);
	DelayMS(10);
	if(getCmdResponse(resp_buff) < 0){
		return 2;
	}

	return 0;
}

uint8_t NCP_GetRemoteLED(char *ip, int port)
{
	if(ip == NULL){
		return 2;
	}

	uint8_t resp_buff[32] = { 0 };

	NCP_ReceiveStart();
	sprintf((char*)uart_command,"http_get http://%s/command/gge\%204\r\n",ip);
	size_t data_len = strlen((char*)uart_command);
	DEFINE_NCPDATA(data,uart_command,data_len);
	NCP_SendData(&data);
	DelayMS(10);

	sprintf((char*)uart_command,"stream_read 0 60\r\n");
	data_len = strlen((char*)uart_command);
	data.bufferLen = data_len;
	NCP_SendData(&data);
	DelayMS(10);

	data_len = getCmdResponse(resp_buff);
	if( data_len < 0){
		return 3;
	}

	int state = atoi(&resp_buff[data_len-1]);
	return (uint8_t)state;
}


void set_machine_mode()
{
	size_t command_len;
	uint8_t resp_buff[128] = { 0 };

	sprintf((char*)uart_command, "set system.cmd.echo off\r\n");
	command_len = strlen((char*)uart_command);
	DEFINE_NCPDATA(data,uart_command,command_len);
	NCP_SendData(&data);
	DelayMS(10);
	getCmdResponse(resp_buff);

	sprintf((char*)uart_command, "set system.print_level 0\r\n");
	command_len = strlen((char*)uart_command);
	data.bufferLen = command_len;
	NCP_SendData(&data);
	DelayMS(10);
	getCmdResponse(resp_buff);

	sprintf((char*)uart_command, "set system.cmd.header_enabled 1\r\n");
	command_len = strlen((char*)uart_command);
	data.bufferLen = command_len;
	NCP_SendData(&data);
	DelayMS(10);
	getCmdResponse(resp_buff);

	sprintf((char*)uart_command, "set system.cmd.prompt_enabled 0\r\n");
	command_len = strlen((char*)uart_command);
	data.bufferLen = command_len;
	NCP_SendData(&data);
	DelayMS(10);
	getCmdResponse(resp_buff);

	sprintf((char*)uart_command, "save\r\n");
	command_len = strlen((char*)uart_command);
	data.bufferLen = command_len;
	NCP_SendData(&data);
	DelayMS(5000);
	getCmdResponse(resp_buff);

}

uint32_t get_NetworkEpochTime(uint8_t *err)
{
	size_t command_len;
	unsigned char buffer[30] = {0};

	NCP_ReceiveStart();
	sprintf((char*)uart_command, "get ti r\r\n");
	command_len = strlen((char*)uart_command);
	DEFINE_NCPDATA(data,uart_command,command_len);
	NCP_SendData(&data);
	DelayS(2);
	if(getCmdResponse(buffer) < 0){
		LOG_ERROR("Get EpochTime\r\n");
		*err = 1;
	}
	else{
		*err = 0;
	}
	return (uint32_t)strtoumax((char*)buffer,NULL,10);
}

int getCmdResponse(unsigned char* uart_reponse)
{
    uint8_t header[16] = { 0 };

    if(NCP_ReceiveBlocking(header, 9)){
    	return -1;
    }

    if(header[0] == 'R')
    {
        if(header[1] == '0')
        {
            int len = atoi((char *)&header[2]);

            // every response always ends in /r/n (i.e., always > 2 bytes)
            if(len > 2)
            {
                unsigned char temp[2];

                // read the data (without the trailing /r/n)
                if(NCP_ReceiveBlocking(uart_reponse, len - 2)){
                	return -1;
                }

                // cleanup the trailing /r/n
                if(NCP_ReceiveBlocking(temp, 2)){
                	return -1;
                }
                // return actual data length
                return len - 2;
            }
            else
            {
                return 0;
            }
        }
        else{
        	int len = atoi((char *)&header[2]);
        	// read the data (without the trailing /r/n)
        	NCP_ReceiveBlocking(uart_reponse, len);
        }
    }
    printf("[NCP GET CMD RESP ERROR] Header:%s\r\n",header);
    return -1;
}

//do not use this
void NCP_FlushRecv()
{
	char temp[100];
	NCP_ReceiveBlocking((uint8_t*)temp, 100);
}


int MQTT_publish(char *ip_addr, int port, const struct sensorNodeDetails *node_data)
{
	if(node_data == NULL || ip_addr == NULL){
		LOG_ERROR("Invalid Params in %s\r\n",__FUNCTION__);
		while(1);
	}
	/*
	 * TODO:
	 * Change to multiple functions,
	 * 1. to open http post stream and return handle
	 * 2.
	 */
	//TODO: return proper error codes
	size_t data_len;
	unsigned char uartResponse[30] = {0};
	char serializedPacket[128]= {0};
	uint8_t handle = 0;

	int ret = SerializeSensorNodeData(serializedPacket, sizeof(serializedPacket), node_data);
	if(ret < 0){
		printf("Serialize Packet Error\r\n");
		while(1);
	}

	size_t packetLen = strlen((char*)serializedPacket);
	NCP_ReceiveStart();
	sprintf((char*)uart_command,"http_post %s:%d -l %lu\r\n",ip_addr,port,packetLen);
	data_len = strlen((char*)uart_command);
	DEFINE_NCPDATA(data,uart_command,data_len);
	NCP_SendData(&data);
	DelayMS(10);
	if(getCmdResponse(uartResponse) < 0) return -1;

	DelayMS(10);

	sprintf((char*)uart_command,"write %u %u\r\n",handle,packetLen);
	data_len = strlen((char*)uart_command);
	data.bufferLen = data_len;
	NCP_SendData(&data);
//	UDELAY_Delay(COMMAND_RESPONSE_DELAY);
//	if(getCmdResponse(uartResponse) < 0) return -1;

	DelayMS(10);

	sprintf((char*)uart_command,"%s",serializedPacket);
	data_len = strlen((char*)uart_command);
	data.bufferLen = data_len;
	NCP_SendData(&data);
	DelayMS(100);
	if(getCmdResponse(uartResponse) < 0) return -1;

	DelayS(2);

	sprintf((char*)uart_command,"stream_close %d\r\n",handle);
	data_len = strlen((char*)uart_command);
	data.bufferLen = data_len;
	NCP_SendData(&data);
	DelayMS(100);
	if(getCmdResponse(uartResponse) < 0) return -1;

	return 0;
}
