/*
 * httpHandler.h
 *
 *  Created on: May 19, 2016
 *      Author: Danius
 */

#ifndef INCLUDE_HTTPHANDLER_H_
#define INCLUDE_HTTPHANDLER_H_

#include <espconn.h>
#include "jsmn.h"
#include "httpParser.h"

#define HTML_POS 0x12000
#define HTML_SIZE 4869
#define CHUNK_SIZE 2048

typedef struct espconn espconn;

struct callbackParams{
	espconn *pEspConn;
	uint8 amountOfSends;
	uint8 amountOfSent;
	uint16 sendRemainder;
	bool chunkedSend;
	uint8 *flashDataAddr;
};

typedef struct callbackParams callbackParams;

void handleGet(espconn *pEspConn, httpHeaderStruct *header);

void handlePost(espconn *pEspConn, httpHeaderStruct *header, char *postData, int dataLength);

void sendData(struct espconn *pEspConn, uint8 *header, uint32 headerLength,
		uint8 *dataAddrFlash, uint32 dataLength);

void sendDataChunk(struct espconn *pEspConn, uint8 *dataAddrFlash, uint32 dataLength);

void sentCB(void *arg);

void readFlash(char *dst, char *src, int len);

void readFlashUnaligned(char *dst, char *src, int len);

void scanCB(void *arg, STATUS status);

static int jsoneq(const char *json, jsmntok_t *tok, const char *s);

void connectWifi(espconn *pEspConn, char *postData, int dataLength);

#endif /* INCLUDE_HTTPHANDLER_H_ */
