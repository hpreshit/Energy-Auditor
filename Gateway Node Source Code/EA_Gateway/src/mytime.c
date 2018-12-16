/*
 * mytime.c
 *
 *  Created on: 15-Dec-2018
 *      Author: Gunj Manseta
 */

#include "em_core.h"
#include "em_rtcc.h"
#include "em_cmu.h"

#include "mytime.h"
#include "logger.h"

static volatile uint32_t time_epochTime = 0;

extern volatile bool standby;
extern volatile uint8_t watchActiveGatewayCount;

static volatile uint8_t count = 0;

void TimeInit()
{
	CMU_ClockEnable(cmuClock_RTCC, true);

	RTCC_Init_TypeDef rtccInit = RTCC_INIT_DEFAULT;
	rtccInit.enable                = true;
	rtccInit.debugRun              = false;
	rtccInit.precntWrapOnCCV0      = false;
	rtccInit.cntWrapOnCCV1         = true;
	rtccInit.prescMode             = rtccCntTickPresc;
	rtccInit.presc                 = rtccCntPresc_32;
	rtccInit.enaOSCFailDetect      = false;
	rtccInit.cntMode               = rtccCntModeNormal;

	RTCC_CCChConf_TypeDef rtccCompareInit = RTCC_CH_INIT_COMPARE_DEFAULT;
	RTCC_ChannelInit(1,&rtccCompareInit);
	RTCC_ChannelCCVSet(1,1024);

	RTCC_IntClear(0xFFFFFFFF);
	RTCC_IntEnable(RTCC_IEN_CC1);
	NVIC_ClearPendingIRQ(RTCC_IRQn);
	NVIC_EnableIRQ(RTCC_IRQn);

	RTCC_Init(&rtccInit);
}

void RTCC_IRQHandler()
{
	CORE_CRITICAL_SECTION(
		RTCC_IntClear(RTCC_IFC_CC1);
		RTCC_CounterSet(0);
		++time_epochTime;
//		if(standby && count == 5){
//			count = 0;
//			((watchActiveGatewayCount == 0) ? standby = false : --watchActiveGatewayCount);
//		}
//		else if(standby){
//			count++;
//		}
	);
}

uint32_t TimeGet()
{
	volatile uint32_t time = 0;
	CORE_CRITICAL_SECTION(
		time = time_epochTime;
	);
	return time;
}

void TimeSet(uint32_t time)
{
	CORE_CRITICAL_SECTION(
		time_epochTime = time;
	);
}

void DelayMS(uint32_t ms)
{
	if(ms >= 1000){
		DelayS(ms/1000);
		ms = ms%1000;
	}
	if(ms == 0){
		return;
	}
	volatile uint32_t nowCnt = RTCC_CounterGet();
	volatile uint32_t target = nowCnt + ms;
	volatile uint32_t thenCnt = 0;
	do{
		thenCnt = RTCC_CounterGet();
		if(thenCnt < nowCnt){
			thenCnt = thenCnt + 1000;
		}
	}while(thenCnt < target);
}

void DelayS(uint32_t sec)
{
	if(sec == 0){
		return;
	}
	volatile uint32_t target = TimeGet() + sec;
	volatile uint32_t now = 0;
	while(1){
		now = TimeGet();
		if(now >= target){
			break;
		}
	}
}
