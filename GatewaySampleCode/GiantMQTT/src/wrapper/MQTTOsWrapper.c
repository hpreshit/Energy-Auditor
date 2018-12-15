/*******************************************************************************
 * Copyright (c) 2014, 2015 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 ******************************************************************************/

#include "MQTTOsWrapper.h"
#include "udelay.h"

#define _ISOC99_SOURCE
#include <inttypes.h>


// Define receive/transmit operation queues
DEFINE_BUF_QUEUE(EMDRV_UARTDRV_MAX_CONCURRENT_RX_BUFS, rxBufferQueue);
DEFINE_BUF_QUEUE(EMDRV_UARTDRV_MAX_CONCURRENT_TX_BUFS, txBufferQueue);

// For ZentriOS-WL, max UART recv size is 1000. Setting to 1024 is always enough
#define UART_RX_FIFO_SIZE           1024

// Configuration for USART0
#define UART_INTERFACE                                                         \
{                                                                              \
    USART0,                                                                    \
    115200,                                                                    \
	_USART_ROUTELOC0_TXLOC_LOC23,													\
	_USART_ROUTELOC0_RXLOC_LOC31, 												\
    usartStopbits1,                                                            \
    usartNoParity,                                                             \
    usartOVS16,                                                                \
    false,                                                                     \
    uartdrvFlowControlNone,                                                    \
    gpioPortA,                                                                 \
    0,                                                                        \
    gpioPortD,                                                                 \
    15,                                                                        \
    (UARTDRV_Buffer_FifoQueue_t *)&rxBufferQueue,                              \
    (UARTDRV_Buffer_FifoQueue_t *)&txBufferQueue,                               \
	0,\
	0\
}

static const RTCC_Init_TypeDef initRTC =
{
  true,  // Start counting when init completed.
  false, // Disable updating RTC during debug halt.
  false  // Count until max. to wrap around.
};

static uint8_t uart_rx_fifo[UART_RX_FIFO_SIZE] = {0};
static uint8_t uart_command[128];
static UARTDRV_HandleData_t handle_data;
static UARTDRV_Handle_t uart_handle = &handle_data;
/******************************************************************************
 * @brief  Static functions
 *****************************************************************************/

#if 0
static void uartTxCallback(UARTDRV_Handle_t handle,
              Ecode_t transferStatus,
              uint8_t *data,
              UARTDRV_Count_t transferCount)
{
}
#endif
static void uartRxCallback(UARTDRV_Handle_t handle,
              Ecode_t transferStatus,
              uint8_t *data,
              UARTDRV_Count_t transferCount)
{
}

static void delayTicks( uint32_t ticks )
{
    volatile uint32_t now;
    uint32_t startTime;

    if ( ticks )
    {
        startTime = RTCC_CounterGet();
        do
        {
            now = RTCC_CounterGet();
        } while ( TIMEDIFF( now, startTime ) < ticks );
    }
}

static void initRealTime( void )
{
    // Ensure LE modules are clocked.
    CMU_ClockEnable( cmuClock_CORELE, true );

    // Enable LFACLK in CMU (will also enable oscillator if not enabled).
    CMU_ClockSelectSet( cmuClock_LFA, RTC_OSC );

    // Set clock divider.
    CMU_ClockDivSet( cmuClock_RTCC, RTC_DIVIDER );

    // Enable RTC module clock.
    CMU_ClockEnable( cmuClock_RTCC, true );

    // Initialize RTC.
    RTCC_Init( &initRTC );

    // Disable RTC/RTCC interrupt generation.
    RTCC_IntDisable( RTC_ALL_INTS );
    RTCC_IntClear( RTC_ALL_INTS );

    //RTCC_CounterReset();
    RTCC_Reset();

    // Clear and then enable RTC interrupts in NVIC.
    NVIC_ClearPendingIRQ(RTCC_IRQn);
    NVIC_EnableIRQ(RTCC_IRQn);
}

static void ReceiveBlocking(UARTDRV_Handle_t handle, uint8_t *data, UARTDRV_Count_t count)
{
    static UARTDRV_Count_t consumed = 0;

    UARTDRV_Count_t received;
    UARTDRV_Count_t remaining;
    uint8_t *buffer = NULL;
    uint8_t retry = 0;

    // block here waiting till required data size is received
    do
    {
        UARTDRV_GetReceiveStatus(uart_handle, &buffer, &received, &remaining);

        // we already have available data that is not yet consumed.. return it
        if(count <= received-consumed)
        {
            memcpy(data, &uart_rx_fifo[consumed], count);
            consumed += count;

            // now it is a good time to reset buffers and counters
            if(received == consumed)
            {
                UARTDRV_Abort(uart_handle, uartdrvAbortReceive);

                // fill in the buffer starting from first empty location;
                consumed = 0;
                UARTDRV_Receive(uart_handle, uart_rx_fifo, UART_RX_FIFO_SIZE, uartRxCallback);
            }
            break;
        }
//        Sleep_ms(20);  // remember, others need to live as well
        UDELAY_Delay(20000);
        retry++;
    } while(true && retry < 50);

}

static int getCmdResponse(unsigned char* uart_reponse)
{
    uint8_t header[16] = { 0 };

    ReceiveBlocking(uart_handle, header, 9);

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
                ReceiveBlocking(uart_handle, uart_reponse, len - 2);

                // cleanup the trailing /r/n
                ReceiveBlocking(uart_handle, temp, 2);

                // return actual data length
                return len - 2;
            }
            else
            {
                return 0;
            }
        }
    }
    return -1;
}

/******************************************************************************
 * @brief  API's
 *****************************************************************************/
void Sleep_ms( uint32_t ms )
{
    uint64_t totalTicks = MSEC_TO_TICKS( ms );

    while ( totalTicks > RTC_CLOSE_TO_MAX_VALUE )
    {
        delayTicks( RTC_CLOSE_TO_MAX_VALUE );
        totalTicks -= RTC_CLOSE_TO_MAX_VALUE;
    }
    delayTicks( totalTicks );
}

void TimerInit(Timer* timer)
{
    timer->end_time = 0;
}

char TimerIsExpired(Timer* timer)
{
    int64_t diff;
    uint64_t now;
    uint32_t currentCnt = RTCC_CounterGet();
    now = TICKS_TO_MSEC(currentCnt);
    diff = timer->end_time - now;
    return (diff < 0) || (diff == 0);
}

void TimerCountdownMS(Timer* timer, unsigned int timeout)
{
    uint64_t now;
    uint32_t currentCnt = RTCC_CounterGet();
    now = TICKS_TO_MSEC(currentCnt);
    timer->end_time = timeout + now;
}

void TimerCountdown(Timer* timer, unsigned int timeout)
{
    uint64_t now;
    uint32_t currentCnt = RTCC_CounterGet();
    now = TICKS_TO_MSEC(currentCnt);
    timer->end_time = (timeout * 1000) + now;
}

int TimerLeftMS(Timer* timer)
{
    int64_t diff;
    uint64_t now;
    uint32_t currentCnt = RTCC_CounterGet();
    now = TICKS_TO_MSEC(currentCnt);
    diff = timer->end_time - now;
    return (diff < 0) ? 0 : diff;
}

int OsWrapper_read(Network* n, unsigned char* buffer, int len, int timeout_ms)
{
    int bytes = 0;
    size_t command_len;
    uint64_t now, end;
    now = TICKS_TO_MSEC(RTCC_CounterGet());
    end = now + timeout_ms;

#ifdef BUS_COMMAND_MODE
    while(bytes < len)
    {
        sprintf((char*)uart_command, "read %u %u\r\n", n->my_socket, len - bytes);
        command_len = strlen((char*)uart_command);

        if(UARTDRV_TransmitB(uart_handle, uart_command, command_len) != ECODE_OK)
            return -1;

        bytes += getCmdResponse(buffer+bytes);

        now = TICKS_TO_MSEC(RTCC_CounterGet());
        if(end < now)   // Timeout
        {
            break;
        }

        // slow down, give chance to other processes
//        Sleep_ms(100);
        UDELAY_Delay(100000);
    }
#else
    ReceiveBlocking(uart_handle, buffer, len);
    bytes = len;

    // ToDo: should check for timeout here as well
#endif

    return bytes;
}

int OsWrapper_write(Network* n, unsigned char* buffer, int len, int timeout_ms)
{
#ifdef BUS_COMMAND_MODE
    size_t command_len;
    unsigned char read_buf[128] = { 0 };

    sprintf((char*)uart_command, "write %u %u\r\n", n->my_socket, len);
    command_len = strlen((char*)uart_command);
    if(UARTDRV_TransmitB(uart_handle, uart_command, command_len) != ECODE_OK)
        return -1;
#endif

    if(UARTDRV_TransmitB(uart_handle, buffer, len) != ECODE_OK)
        return -1;

#ifdef BUS_COMMAND_MODE
    // cleanup ZentriOS response
    getCmdResponse(read_buf);
#endif

    // ToDo: should return only the succeeded transmitted length, not the input len
    return len;
}

void NetworkInit(Network* n)
{
    n->my_socket = 0;
    n->mqttread = OsWrapper_read;
    n->mqttwrite = OsWrapper_write;
    n->disconnect = NetworkDisconnect;

    initRealTime();

    // Initialize driver handle
    UARTDRV_InitUart_t initData = UART_INTERFACE;
    Ecode_t ecode = UARTDRV_InitUart(uart_handle, &initData);
    if(ecode != ECODE_EMDRV_UARTDRV_OK){
    	while(1);
    }
    // Initialize UART receive routine, this is a non blocking call
    ecode = UARTDRV_Receive(uart_handle, uart_rx_fifo, UART_RX_FIFO_SIZE, uartRxCallback);
    if(ecode != ECODE_EMDRV_UARTDRV_OK){
		while(1);
	}
}

int NetworkConnect(Network* n, char* addr, int port)
{
#ifdef BUS_COMMAND_MODE
    size_t command_len;
    uint8_t resp_buff[128] = { 0 };

    ReceiveBlocking(uart_handle, resp_buff, 128);
    printf("%s \n",(char *)resp_buff);
    //ReceiveBlocking(uart_handle, header, 9);

    /*
    sprintf((char*)uart_command, "ping google.com\r\n");
    command_len = strlen((char*)uart_command);
    if(UARTDRV_TransmitB(uart_handle, uart_command, command_len) != ECODE_OK)
            return -1;
    UDELAY_Delay(500000);
    getCmdResponse(resp_buff);
    printf("%s \n",(char *)resp_buff);
    ///
	*/

    if(port == MQTT_SECURE_PORT)
    {
        // secure - use TLS
        sprintf((char*)uart_command, "tls_client %s %u\r\n", addr, port);
    }
    else
    {
        // clear - use TCP
        sprintf((char*)uart_command, "tcp_client %s %u\r\n", addr, port);
    }
    command_len = strlen((char*)uart_command);

    if(UARTDRV_TransmitB(uart_handle, uart_command, command_len) != ECODE_OK)
        return -1;
    UDELAY_Delay(500000);
    if(getCmdResponse(resp_buff) > 0)
    {
        n->my_socket = atoi((char *)resp_buff);
    }
    else
    {
        n->my_socket = -1;
    }

    return n->my_socket;
#else
    return 0;
#endif
}

void NetworkDisconnect(Network* n)
{
#ifdef BUS_COMMAND_MODE
    size_t command_len;
    unsigned char read_buf[128] = { 0 };

    sprintf((char*)uart_command, "close %u\r\n", n->my_socket);
    command_len = strlen((char*)uart_command);

    UARTDRV_TransmitB(uart_handle, uart_command, command_len);

    // cleanup ZentriOS response
    getCmdResponse(read_buf);
#else
    return;
#endif
}

void set_machine_mode(void)
{
	size_t command_len;
	uint8_t resp_buff[128] = { 0 };

	sprintf((char*)uart_command, "set system.print_level 0\r\n");
	command_len = strlen((char*)uart_command);
	UARTDRV_TransmitB(uart_handle, uart_command, command_len);
	UDELAY_Delay(10000);
	getCmdResponse(resp_buff);

	sprintf((char*)uart_command, "set system.cmd.header_enabled 1\r\n");
	command_len = strlen((char*)uart_command);
	UARTDRV_TransmitB(uart_handle, uart_command, command_len);
	UDELAY_Delay(10000);
	getCmdResponse(resp_buff);

	sprintf((char*)uart_command, "set system.cmd.prompt_enabled 0\r\n");
	command_len = strlen((char*)uart_command);
	UARTDRV_TransmitB(uart_handle, uart_command, command_len);
	UDELAY_Delay(10000);
	getCmdResponse(resp_buff);

	sprintf((char*)uart_command, "set system.cmd.echo off\r\n");
	command_len = strlen((char*)uart_command);
	UARTDRV_TransmitB(uart_handle, uart_command, command_len);
	UDELAY_Delay(10000);
	getCmdResponse(resp_buff);
}

uint32_t get_time(void)
{
	size_t command_len;
	unsigned char buffer[30] = {0};

	sprintf((char*)uart_command, "get ti r\r\n");
	command_len = strlen((char*)uart_command);
	UARTDRV_TransmitB(uart_handle, uart_command, command_len);
	UDELAY_Delay(COMMAND_RESPONSE_DELAY);
	if(getCmdResponse(buffer) < 0) return -1;

	return (uint32_t)strtoumax((char*)buffer,NULL,10);
}

int MQTT_publish(char * ip_addr,int port,const struct SensorNodeDetails *node_data)
{
	/*
	 * TODO:
	 * Change to multiple functions,
	 * 1. to open http post stream and return handle
	 * 2.
	 */
	size_t data_len;
	unsigned char buffer[30] = {0};
	char data[80] = {0};
	uint8_t handle = 0;

	//sprintf((char*)data,"%d,%d",node,current);
	MQTT_create_packet(data,node_data);

	data_len = strlen((char*)data);

	sprintf((char*)uart_command,"http_post %s:%d -l %d\r\n",ip_addr,port,data_len);
	data_len = strlen((char*)uart_command);
	UARTDRV_TransmitB(uart_handle, uart_command, data_len);
	UDELAY_Delay(COMMAND_RESPONSE_DELAY);
	if(getCmdResponse(buffer) < 0) return -1;

	data_len = strlen((char*)data);
	sprintf((char*)uart_command,"write %u %u\r\n",handle,data_len);
	data_len = strlen((char*)uart_command);
	UARTDRV_TransmitB(uart_handle, uart_command, data_len);
	//UDELAY_Delay(COMMAND_RESPONSE_DELAY);
	//if(getCmdResponse(buffer) < 0) return -1;

	sprintf((char*)uart_command,"%s\r\n",data);
	data_len = strlen((char*)uart_command);
	UARTDRV_TransmitB(uart_handle, uart_command, data_len);
	UDELAY_Delay(COMMAND_RESPONSE_DELAY);
	if(getCmdResponse(buffer) < 0) return -1;

	sprintf((char*)uart_command,"stream_close %d\r\n",handle);
	data_len = strlen((char*)uart_command);
	UARTDRV_TransmitB(uart_handle, uart_command, data_len);
	UDELAY_Delay(COMMAND_RESPONSE_DELAY);
	getCmdResponse(buffer);
	if(getCmdResponse(buffer) < 0) return -1;

	return 0;
}

#define DATA_PACKET_FMT	"{\"node_id\":%d,\"ts\":%d,\"current\":%d,\"unit\":\"%s\",\"connected\":%d,\"stale_read\":%d}"
#define EXTRACT_TIMESTAMP(packet) (packet->timestamp)
#define EXTRACT_CURRENT(packet) (packet->current)
#define EXTRACT_NODE_ID(packet) (packet->nodeid)
#define EXTRACT_CONNECTED(packet) (packet->connected?1:0)
#define EXTRACT_STALE_READ(packet) (packet->stale_read?1:0)
#define CURRENT_UNIT "mA"


int MQTT_create_packet(char *buffer, const struct SensorNodeDetails *node_data)
{
	/* Change int in above parameter and make it the BT struct.
	 * get all the required data from struct
	 * and add as comma separated into the buffer
	 *
	 */

	sprintf((char*)buffer,DATA_PACKET_FMT,EXTRACT_NODE_ID(node_data),EXTRACT_TIMESTAMP(node_data),EXTRACT_CURRENT(node_data),\
		CURRENT_UNIT, EXTRACT_CONNECTED(node_data),EXTRACT_STALE_READ(node_data));

	return 0;
}


