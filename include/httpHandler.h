/*
 * httpHandler.h
 *
 *  Created on: May 19, 2016
 *      Author: Danius
 */

#ifndef INCLUDE_HTTPHANDLER_H_
#define INCLUDE_HTTPHANDLER_H_

#include "httpParser.h"
#include <espconn.h>

#define HTML_POS 0x12000
#define HTML_SIZE 4269

typedef struct espconn espconn;

void handleGet(espconn *pEspConn, httpHeaderStruct *header);

void readFlashUnaligned(char *dst, char *src, int len);

void scanCB(void *arg, STATUS status);

#endif /* INCLUDE_HTTPHANDLER_H_ */
