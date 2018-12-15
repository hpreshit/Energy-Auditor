/*
 * ldma.h
 *
 *  Created on: Dec 13, 2018
 *      Author: Preshit
 */

#ifndef SRC_LDMA_H_
#define SRC_LDMA_H_

#include "letimer.h"
#include "em_ldma.h"
#include "adc0.h"

#define LDMA_CHANNEL      0
#define ADC_BUFFER_SIZE   4

LDMA_TransferCfg_t trans;
LDMA_Descriptor_t descr;

void LDMA_init(void);
void LDMA_IRQHandler(void);


#endif /* SRC_LDMA_H_ */
