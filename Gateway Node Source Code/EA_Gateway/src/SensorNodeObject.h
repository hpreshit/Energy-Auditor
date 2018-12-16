/*
 * SensorNodeObject.h
 *
 *  Created on: 14-Dec-2018
 *      Author: Gunj Manseta
 */

#ifndef SRC_SENSORNODEOBJECT_H_
#define SRC_SENSORNODEOBJECT_H_

#include <stdbool.h>

#define MAX_SENSOR_NODES 8

struct sensorNodeDetails{
	uint32_t sensorNodeIndex;
	uint8_t btAddr[4];
	uint32_t epochTime[2];
	uint16_t currentReading[2];
	bool staleReading;
	bool sensorNodeConnected;
};

#define DATA_PACKET_JSON			"{\"node_id\":%lu,\"ts\":%lu,\"btaddr\":\"%x:%x:%x:%x\",\"current\":%u,\"unit\":\"%s\",\"connected\":%u,\"stale_read\":%u}"
#define EXTRACT_TIMESTAMP(packet)	(packet->epochTime[1])
#define EXTRACT_CURRENT(packet) 	(packet->currentReading[1])
#define EXTRACT_NODE_ID(packet) 	(packet->sensorNodeIndex)
#define EXTRACT_CONNECTED(packet) 	(packet->sensorNodeConnected)
#define EXTRACT_STALE_READ(packet) 	(packet->staleReading)
#define EXTRACT_BT_ADDR(packet) 	packet->btAddr[3], packet->btAddr[2],packet->btAddr[1],packet->btAddr[0]
#define CURRENT_UNIT 				"mA"

static inline int SerializeSensorNodeData(char *buffer, size_t bufferSize, const struct sensorNodeDetails *node_data)
{
	if(buffer == NULL || node_data == NULL){
		while(1);
	}
	int ret = snprintf((char*)buffer, bufferSize, DATA_PACKET_JSON,
			EXTRACT_NODE_ID(node_data),
			EXTRACT_TIMESTAMP(node_data),
			EXTRACT_BT_ADDR(node_data) ,
			EXTRACT_CURRENT(node_data),
			CURRENT_UNIT,
			EXTRACT_CONNECTED(node_data),
			EXTRACT_STALE_READ(node_data)
			);

	return ret;
}

#endif /* SRC_SENSORNODEOBJECT_H_ */
