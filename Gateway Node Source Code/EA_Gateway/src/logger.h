/*
 * logger.h
 *
 *  Created on: 15-Dec-2018
 *      Author: Gunj Manseta
 */

#ifndef SRC_LOGGER_H_
#define SRC_LOGGER_H_

#define DEBUG_LOG

#define ERROR "[ERROR] "
#define OK "[OK] "
#define INFO "[INFO] "
#define LOG_ERROR(fmt, ...) printf(ERROR " %s "  fmt, __FUNCTION__, ##__VA_ARGS__)
#define LOG_OK(fmt, ...) 	printf(OK fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) 	printf(INFO fmt, ##__VA_ARGS__)

#ifdef DEBUG_LOG
#define DEBUG "[DEBUG] "
#define LOG_DEBUG(fmt, ...) 	printf(DEBUG fmt, ##__VA_ARGS__)
#else
#define LOG_DEBUG(fmt, ...)
#endif


#endif /* SRC_LOGGER_H_ */
