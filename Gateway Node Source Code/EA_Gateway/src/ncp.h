/*
 * ncp.h
 *
 *  Created on: 14-Dec-2018
 *      Author: Gunj Manseta
 */

#ifndef SRC_NCP_H_
#define SRC_NCP_H_

#include "SensorNodeObject.h"

//#define PROXY_IP					"10.0.0.144"	//gunj laptop
//#define PROXY_IP					"10.0.0.200"	//aakash laptop
#define PROXY_IP					"10.0.0.215"	//pi
#define PROXY_PORT					8000

void NCP_FlushRecv();
void set_machine_mode();
void NCPInit();
uint32_t get_NetworkEpochTime();
int MQTT_publish(char *ip_addr, int port, const struct sensorNodeDetails *node_data);
int MQTT_RestartReasonPublish(char *ip_addr, int port, const char *restartReason);

#endif /* SRC_NCP_H_ */
