/*
 * letimer.c
 *
 *  Created on: Jan 27, 2018
 *      Author: Preshit
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "em_rtcc.h"
#include "letimer.h"
#include "native_gecko.h"
/* GATT database */
#include "gatt_db.h"
//#include <lcd-graphics/graphics.h>
//***********************************************************************************
// global variables
//***********************************************************************************
uint16_t prescalar    			= 1;
volatile uint32_t adcSampleValue= 0;
volatile uint32_t max_adcValue 	= 0;
volatile uint32_t avg_adcValue	= 0;
uint32_t ondutycount  			= 0;
uint32_t periodcount  			= 0;
uint16_t previousCurrentValue	= 0;
volatile uint32_t clockFreq 	= 0;
volatile uint16_t sampleCount 	= 0;
volatile uint16_t secondsCount	= 0;
volatile uint32_t g_ADCepochTime= 0;
//***********************************************************************************
// function prototypes
//***********************************************************************************
void LETIMER_init(){
	LETIMER_Enable(LETIMER0,false);								//Disable LETIMER0

	const LETIMER_Init_TypeDef letimer0Init =
	{
	.enable = false,
	.debugRun = false,
	.comp0Top = true,
	.bufTop = false,
	.out0Pol = 0,
	.out1Pol = 0,
	.ufoa0 = letimerUFOANone,
	.ufoa1 = letimerUFOANone,
	.repMode = letimerRepeatFree
	};

	LETIMER_Init(LETIMER0,&letimer0Init);						//Initialize LETIMER0

	// Set prescalar
	LETIMER_setPrescalar();

	// COMP0 and COMP1 values
	LETIMER_setCompValue();
}

void LETIMER_setPrescalar(){
	uint16_t countermax = 0xFFFF;

	clockFreq = CMU_ClockFreqGet(cmuClock_LETIMER0);			//Get Clock frequency for LETIMER0
	while(LETIMER0_Period>(countermax*prescalar/clockFreq)){	//calculate prescalar
		prescalar<<=1;
	}
	//CMU->LFAPRESC0 = prescalar;
	CMU_ClockDivSet(cmuClock_LETIMER0, prescalar);				//Set prescalar
}

void LETIMER_setCompValue(){
	periodcount = 0;
	ondutycount = 0;

	periodcount =((LETIMER0_Period*clockFreq)/(prescalar));			//calculate count for period
	ondutycount =((LETIMER0_On_Time*(float)clockFreq)/(prescalar*1000));	//calculate count for on-duty cycle

	LETIMER_CompareSet(LETIMER0,0,periodcount);						//Set COMP0 register value
}

void LETIMER_enable(){
	CORE_AtomicDisableIrq();
	sampleCount = 0;
	max_adcValue = 0;
	CORE_AtomicEnableIrq();
	//Clear Interrupt flags
	LETIMER0->IFC |= LETIMER_IFC_REP1|LETIMER_IFC_REP1|LETIMER_IFC_UF|LETIMER_IFC_COMP1|LETIMER_IFC_COMP0;

	//Enable COMP0 interrupt
	LETIMER_IntEnable(LETIMER0,LETIMER_IF_UF);
	LETIMER_IntDisable(LETIMER0,LETIMER_IF_COMP1);

	//blocksleepmode(letimer_minimumMode);

	NVIC_EnableIRQ(LETIMER0_IRQn);		//Enable LETIMER0 Interrupt

	LETIMER_Enable(LETIMER0,true);	    //Enable LETIMER0
}

void LETIMER_disable(){
	LETIMER0->CMD = LETIMER_CMD_STOP | LETIMER_CMD_CLEAR;
//	LETIMER_Enable(LETIMER0,false);
}

void LETIMER0_IRQHandler(void){
	CORE_AtomicDisableIrq();
	uint32_t flagInterrupt = LETIMER_IntGet(LETIMER0);
	if(flagInterrupt & LETIMER_IF_COMP1){
		LETIMER_IntClear(LETIMER0,LETIMER_IF_COMP1);	//clear comp1 interrupt flag
		sampleCount++;
		LDMA_StartTransfer(LDMA_CHANNEL, &trans, &descr);
//		ADC0_stop();
		LETIMER_CompareSet(LETIMER0,1,(periodcount-(ondutycount*(sampleCount+1)))); 		//Set COMP1 register value
	}

	if(flagInterrupt & LETIMER_IF_UF){
		LETIMER_IntClear(LETIMER0,LETIMER_IF_UF);		//clear underflow interrupt flag
		avg_adcValue = ((avg_adcValue*secondsCount) + max_adcValue)/(secondsCount + 1);
		secondsCount++;
		max_adcValue = 0;
		if(secondsCount==2){
			g_ADCepochTime = RTCC_CounterGet();
			gecko_external_signal(EXT_SIGNAL_SEND_CURRENT_VALUE);
			secondsCount = 0;
		}
//		ADC0_start();
		LETIMER_CompareSet(LETIMER0,1,(periodcount-ondutycount)); 		//Set COMP1 register value
		LETIMER_IntEnable(LETIMER0,LETIMER_IF_COMP1);
	}
	//SLEEP_SleepBlockEnd(sleepEM2);
	CORE_AtomicEnableIrq();
}
