/*
 * ncp_uart.c
 *
 *  Created on: 14-Dec-2018
 *      Author: Gunj Manseta
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ecode.h"
#include "em_cmu.h"
#include "em_rtcc.h"
#include "uartdrv.h"

#include "ncp_uart.h"
#include "mytime.h"
#include "logger.h"

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

// Define receive/transmit operation queues
DEFINE_BUF_QUEUE(1, rxBufferQueue);
DEFINE_BUF_QUEUE(1, txBufferQueue);

// For ZentriOS-WL, max UART recv size is 1000. Setting to 1024 is always enough
#define UART_RX_FIFO_SIZE           1024

static uint8_t uart_rx_fifo[UART_RX_FIFO_SIZE] = {0};
static UARTDRV_HandleData_t handle_data;
static UARTDRV_Handle_t uart_handle = &handle_data;

UARTDRV_Handle_t get_NCPUartHandle(){
	return uart_handle;
}

#if 1
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

void NCP_UARTInit()
{
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

void NCP_SendData(struct NCPData *data)
{
	printf("[NCP_SEND] %s",data->buffer);
	Ecode_t err = UARTDRV_TransmitB(uart_handle, (uint8_t*)data->buffer, data->bufferLen);
	if(err != ECODE_OK){
		printf("[NCP SEND ERROR] Code:%lu\r\n",err);
	}
}

//void NCP_ReceiveBlocking(uint8_t *data, UARTDRV_Count_t count){
//
//	Ecode_t err = UARTDRV_ReceiveB(uart_handle,data,count);
//	if(err != ECODE_OK){
//		printf("[NCP RECV ERROR] Code:%lu\r\n",err);
//	}
//}

void NCP_ReceiveStart()
{
	UARTDRV_Receive(uart_handle, uart_rx_fifo, UART_RX_FIFO_SIZE, uartRxCallback);
}

uint8_t NCP_ReceiveBlocking(uint8_t *data, UARTDRV_Count_t count)
{
    static UARTDRV_Count_t consumed = 0;

    UARTDRV_Count_t received;
    UARTDRV_Count_t remaining;
    uint8_t *buffer = NULL;
    uint32_t retry = 0;

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
        DelayMS(20);
        retry++;
        if(retry > 500){
        	LOG_DEBUG("%s taking too long\r\n",__FUNCTION__);
        	return retry;
        }
    } while(true /*&& retry < 200*/);

    return 0;

}
