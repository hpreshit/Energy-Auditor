/*
 * ncp_uart.h
 *
 *  Created on: 14-Dec-2018
 *      Author: Gunj Manseta
 */

#ifndef SRC_NCP_UART_H_
#define SRC_NCP_UART_H_

#include "uartdrv.h"


struct NCPData{
	char *buffer;
	size_t bufferLen;
};

#define DEFINE_NCPDATA(name, buf, size) struct NCPData name = {.buffer = buf, .bufferLen = size};

void NCP_UARTInit();
void NCP_SendData(struct NCPData *data);
void NCP_ReceiveBlocking(uint8_t *data, UARTDRV_Count_t count);

#endif /* SRC_NCP_UART_H_ */
