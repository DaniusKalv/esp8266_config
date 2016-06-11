/*
 * httpParser.h
 *
 *  Created on: May 19, 2016
 *      Author: Danius
 */

#ifndef INCLUDE_HTTPPARSER_H_
#define INCLUDE_HTTPPARSER_H_

#include <string.h>
#include "c_types.h"

typedef enum _httpMethods
{
	methodUndefined,
	GET,
	POST
} httpMethods;

typedef enum _httpProtocols
{
	protocolUndefined,
	HTTP1_0,
	HTTP1_1,
	HTTP2
} httpProtocols;

struct httpHeaderStruct
{
	httpMethods httpMethod;
	char path[200];
	httpProtocols protocol;
	char host[50];
};

typedef struct httpHeaderStruct httpHeaderStruct;

httpHeaderStruct parseHttp(char *httpHeader, unsigned short headerLength);
int getAmountOfRows(char *text, unsigned short headerLength);
void parseHttpElements(char *text, httpHeaderStruct *receivedHttpHeader);
bool startsWith(const char *str, const char *pre);
int textCopy(int start, char *text, char *dest, char delimiter);

#endif /* INCLUDE_HTTPPARSER_H_ */
