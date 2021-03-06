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

extern uint32_t ondutycount;
extern uint32_t periodcount;
extern uint32_t max_adcValue;
extern uint32_t avg_adcValue;
extern uint32_t adcSampleValue;
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
