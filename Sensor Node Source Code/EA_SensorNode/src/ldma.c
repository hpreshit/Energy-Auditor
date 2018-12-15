/*
 * ldma.c
 *
 *  Created on: Dec 13, 2018
 *      Author: Preshit
 */

#include "ldma.h"

//// Change this to set number of samples.

/**************************************************************************//**
 * @brief LDMA Handler
 *****************************************************************************/
void LDMA_IRQHandler(void)
{
	CORE_AtomicDisableIrq();
	LDMA_StopTransfer(LDMA_CHANNEL);
	// Clear interrupt flag
	LDMA_IntClear(1 << LDMA_CHANNEL << _LDMA_IFC_DONE_SHIFT);
	max_adcValue = max(adcSampleValue,max_adcValue);
	if(sampleCount==20){
		LETIMER_IntDisable(LETIMER0,LETIMER_IF_COMP1);
		sampleCount=0;
	}

	// Insert transfer complete functionality here
	CORE_AtomicEnableIrq();
}

/**************************************************************************//**
 * @brief LDMA initialization
 *****************************************************************************/
void LDMA_init(void)
{
  // Enable LDMA clock
  CMU_ClockEnable(cmuClock_LDMA, true);

  // Basic LDMA configuration
  LDMA_Init_t ldmaInit = LDMA_INIT_DEFAULT;

  LDMA_Init(&ldmaInit);

  // Transfers trigger off ADC single conversion complete
  trans = (LDMA_TransferCfg_t)LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_ADC0_SINGLE);

  descr = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_P2M_BYTE(
      (&(ADC0->SINGLEDATA)),  // source
	  (&adcSampleValue),            // destination
      ADC_BUFFER_SIZE,      // data transfer size
      0);                   // link relative offset (links to self)

  descr.xfer.ignoreSrec = true;       // ignore single requests to reduce time spent out of EM2
  descr.xfer.size = ldmaCtrlSizeWord; // transfer words instead of bytes
  descr.xfer.doneIfs = true;
  descr.xfer.dstInc = ldmaCtrlDstIncNone;

  // Clear pending and enable interrupts for channel
  NVIC_ClearPendingIRQ(LDMA_IRQn);
  NVIC_EnableIRQ(LDMA_IRQn);
}
