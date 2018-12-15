/*
 * adc0.c
 *
 *  Created on: 19-Sep-2017
 *      Author: Gunj Manseta
 */
//#include <sleep_g.h>
#include "adc0.h"
#include "em_adc.h"
#include "em_gpio.h"
#include "cmu.h"
#include "native_gecko.h"
//***********************************************************************************
// defined macros
//***********************************************************************************
#define ADC_CMP_LT_VALUE        				3550                    /* ~2.8V for 3.3V AVDD */
#define ADC_CMP_GT_VALUE        				30                     	/* ~.02V for 3.3V AVDD */
#define JOYSTICK_INPUT							adcPosSelAPORT3XCH8    	/* PA0 */
#define ADC0_INPUT								JOYSTICK_INPUT
#define ADC_CLOCK_PRESCALE_VALUE				111

//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function defintions
//***********************************************************************************

//Only used internally by the ADC0_setup() function so does not have the declaration in the Header file
void ADC0_init(void)
{
	/* Setting up the clock tree for ADC0 */
	cmu_init_ADC0();

	NVIC_DisableIRQ(ADC0_IRQn);
	ADC0->SINGLEFIFOCLEAR = ADC_SINGLEFIFOCLEAR_SINGLEFIFOCLEAR;

	const ADC_Init_TypeDef init = ADC_INIT_DEFAULT;

	const ADC_InitSingle_TypeDef singleInit =
	{
		.posSel = ADC0_INPUT,
		.reference = adcRefVDD,
		.acqTime = adcAcqTime32,
		.rep = true,
		.fifoOverwrite = true,
		.prsEnable = false,
		.negSel = adcNegSelVSS,
		.resolution = adcRes12Bit,
		.diff = false,
		.prsSel = adcPRSSELCh0,
		.singleDmaEm2Wu = true,			//set true for LDMA
		.leftAdjust = false
	};

	/* Change the below value if the ADC is misbehaving */
	//ADC0->BIASPROG |= ADC_BIASPROG_GPBIASACC_LOWACC | ADC_BIASPROG_ADCBIASPROG_SCALE8;

	ADC_Init(ADC0, &init);
	ADC_InitSingle(ADC0, &singleInit);
}

void ADC0_setup(void)
{
	ADC0_init();

	/* Disabling the PA0 as it is needed for the Joystick input */
	GPIO_PinModeSet(gpioPortA,0,gpioModeDisabled,0);
}

void ADC0_start(void)
{
	/* Star the single ended Single mode ADC */
	ADC_Start(ADC0, adcStartSingle);
}

void ADC0_stop()
{
	ADC_Start(ADC0,ADC_CMD_SINGLESTOP | ADC_CMD_SCANSTOP);
	ADC_IntDisable(ADC0, ADC_IEN_SINGLECMP);
	NVIC_DisableIRQ(ADC0_IRQn);
}
