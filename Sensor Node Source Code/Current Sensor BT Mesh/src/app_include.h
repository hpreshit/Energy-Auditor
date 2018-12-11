/*
 * app_include.h
 *
 *  Created on: Nov 27, 2018
 *      Author: Preshit
 */

#ifndef SRC_APP_INCLUDE_H_
#define SRC_APP_INCLUDE_H_

#define LETIMER_MIN_ENERGYSTATE 3
#define LETIMER0_Period 		10			//seconds
#define LETIMER0_On_Time 		1			//milisonds
//#define GATEWAY

#define BLE_ADVERTISING_MIN_MS		(337)
#define BLE_ADVERTISING_MAX_MS		(337)
#define BLE_ADVERTISING_MIN_COUNT	(539)	//count = BLE_ADVERTISING_MIN_MS * 1.6
#define BLE_ADVERTISING_MAX_COUNT	(539)	//count = BLE_ADVERTISING_MAX_MS * 1.6
#define BLE_CONNECTION_MIN_MS		(75)
#define BLE_CONNECTION_MAX_MS		(75)
#define BLE_CONNECTION_MIN_COUNT	(60)	//Count = BLE_CONNECTION_MIN_MS/1.25
#define BLE_CONNECTION_MAX_COUNT	(60)	//Count = BLE_CONNECTION_MAX_MS/1.25
#define BLE_SLAVE_LATENCY_MS		(450)
#define BLE_SLAVE_LATENCY			(5)		//Latency = (BLE_SLAVE_LATENCY_MS/BLE_CONNECTION_MAX_MS)-1
#define BLE_CONNECTION_TIMEOUT_MS	(60)
#define BLE_CONNECTION_TIMEOUT_COUNT (1100)
#define BLE_TX_MAX					(80)
#define BLE_TX_MIN					(-260)

#define max(a,b) (a>b)?(a):(b)

#endif /* SRC_APP_INCLUDE_H_ */
