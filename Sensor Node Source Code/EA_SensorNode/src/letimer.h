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
#include "em_core.h"
#include "sleep.h"
#include "app_include.h"

#define EXT_SIGNAL_SEND_CURRENT_VALUE (0x4)

extern uint32_t ondutycount;
extern uint32_t periodcount;
extern volatile uint32_t max_adcValue;
extern volatile uint32_t avg_adcValue;
//extern volatile uint32_t adcSampleValue;
extern uint16_t previousCurrentValue;
extern volatile uint32_t g_ADCepochTime;
//***********************************************************************************
// function prototypes
//***********************************************************************************
void letimer_init();
void letimer_set_prescalar();
void letimer_set_compvalue();
void letimer_enable();
void letimer_disable();
void letimer_service();
void LETIMER0_IRQHandler(void);
