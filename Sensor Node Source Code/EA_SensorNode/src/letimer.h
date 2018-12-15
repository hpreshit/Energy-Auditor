/*
 * letimer.h
 *
 *  Created on: Jan 27, 2018
 *      Author: Preshit
 */


//***********************************************************************************
// Include files
//***********************************************************************************

#include "em_cmu.h"
#include "em_letimer.h"
#include "adc0.h"
#include "ldma.h"
#include "em_adc.h"
#include "em_core.h"
#include "sleep.h"
#include "app_include.h"

#define EXT_SIGNAL_SEND_CURRENT_VALUE (0x4)

extern uint32_t ondutycount;
extern uint32_t periodcount;
extern volatile uint32_t max_adcValue;
extern volatile uint32_t avg_adcValue;
extern volatile uint32_t adcSampleValue;
extern uint16_t previousCurrentValue;
extern volatile uint32_t g_ADCepochTime;
extern volatile uint16_t sampleCount;

//***********************************************************************************
// function prototypes
//***********************************************************************************
void LETIMER_init();
void LETIMER_setPrescalar();
void LETIMER_setCompValue();
void LETIMER_enable();
void LETIMER_disable();
void LETIMER_service();
void LETIMER0_IRQHandler(void);
