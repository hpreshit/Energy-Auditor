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
 *******************************************************************************/

#if !defined(MQTTOsWrapper_H)
#define MQTTOsWrapper_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ecode.h"
#include "em_cmu.h"
#include "em_rtcc.h"
#include "uartdrv.h"

#if defined(_EFM32_GECKO_FAMILY)
// Assume 32kHz RTC/RTCC clock, cmuClkDiv_2 prescaler, 16 ticks per millisecond
#define RTC_DIVIDER                     ( cmuClkDiv_2 )
#else
// Assume 32kHz RTC/RTCC clock, cmuClkDiv_8 prescaler, 4 ticks per millisecond
#define RTC_DIVIDER                     ( cmuClkDiv_8 )
#endif

#define RTC_ALL_INTS                    ( _RTCC_IF_MASK )
#define RTC_OSC                         ( cmuSelect_LFXO )
#define RTC_CLOCK                       ( 32768U )
#define MSEC_TO_TICKS_DIVIDER           ( 1000U * RTC_DIVIDER )
#define MSEC_TO_TICKS_ROUNDING_FACTOR   ( MSEC_TO_TICKS_DIVIDER / 2 )
#define MSEC_TO_TICKS( ms )             ( ( ( (uint64_t)(ms) * RTC_CLOCK )     \
                                            + MSEC_TO_TICKS_ROUNDING_FACTOR )  \
                                          / MSEC_TO_TICKS_DIVIDER )

#define TICKS_TO_MSEC_ROUNDING_FACTOR   ( RTC_CLOCK / 2 )
#define TICKS_TO_MSEC( ticks )          ( ( ( (uint64_t)(ticks)                \
                                              * RTC_DIVIDER * 1000U )          \
                                            + TICKS_TO_MSEC_ROUNDING_FACTOR )  \
                                          / RTC_CLOCK )

#define RTC_MAX_VALUE                   (_RTCC_CNT_MASK)
#define RTC_CLOSE_TO_MAX_VALUE          (RTC_MAX_VALUE-100UL)
#define TIMEDIFF( a, b )                ((( (a)<<8) - ((b)<<8) ) >> 8 )

#define MQTT_SECURE_PORT                1883
#define MQTT_CLEAR_PORT                 8883	//1883

#define BUS_COMMAND_MODE

#define COMMAND_RESPONSE_DELAY			700000

typedef struct Network Network;

struct Network
{
	int my_socket;
	int (*mqttread) (Network*, unsigned char*, int, int);
	int (*mqttwrite) (Network*, unsigned char*, int, int);
	void (*disconnect) (Network*);
};

typedef struct Timer
{
    int64_t end_time;
} Timer;

//struct SensorNodeDetails
//{
//
//};

void Sleep_ms( uint32_t ms );

void TimerInit(Timer*);
char TimerIsExpired(Timer*);
void TimerCountdownMS(Timer*, unsigned int);
void TimerCountdown(Timer*, unsigned int);
int  TimerLeftMS(Timer*);

int  OsWrapper_read(Network*, unsigned char*, int, int);
int  OsWrapper_write(Network*, unsigned char*, int, int);

void NetworkInit(Network*);
int  NetworkConnect(Network*, char*, int);
void NetworkDisconnect(Network* n);


void set_machine_mode(void);
uint32_t get_time(void);
int MQTT_publish(char * ip_addr,int port,uint16_t node,uint16_t current);
#endif
