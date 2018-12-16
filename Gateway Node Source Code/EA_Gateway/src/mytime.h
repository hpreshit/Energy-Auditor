/*
 * mytime.h
 *
 *  Created on: 15-Dec-2018
 *      Author: Gunj Manseta
 */

#ifndef SRC_MYTIME_H_
#define SRC_MYTIME_H_

void TimeInit();
uint32_t TimeGet();
void TimeSet(uint32_t time);
void DelayS(uint32_t sec);
void DelayMS(uint32_t ms);

#endif /* SRC_MYTIME_H_ */
