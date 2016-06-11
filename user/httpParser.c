/*
 * httpParser.c
 *
 *  Created on: May 19, 2016
 *      Author: Danius
 */

#include "httpParser.h"
#include "c_types.h"
#include <osapi.h>

struct httpHeaderStruct parseHttp(char *httpHeader, unsigned short headerLength)
{
	struct httpHeaderStruct httpHeaderStructure;
	int i, lineStart = 0, amountOfRows;
	amountOfRows = getAmountOfRows(httpHeader, headerLength);

	for(i = 0; i < amountOfRows; i++){
		char buffer[200] = "";
		lineStart = textCopy(lineStart, httpHeader, buffer, '\r') + 2;
		parseHttpElements(buffer, &httpHeaderStructure);
	}
	return httpHeaderStructure;
}

int getAmountOfRows(char *text, unsigned short headerLength)
{
	int rows = 0;
	int i;
	for(i = 0; i < headerLength; i++)
	{
		if(text[i] == '\r') rows++;
	}
	return rows - 1;
}

void parseHttpElements(char *text, httpHeaderStruct *receivedHttpHeader)
{
	int start;
	char buffer[50] = "";
	bool startsWithGet = startsWith(text, "GET ");
	bool startsWithPost = startsWith(text, "POST ");

	if(startsWithGet || startsWithPost){
		if(startsWithGet)
		{
			receivedHttpHeader -> httpMethod = GET;
			start = 4;
		}
		else
		{
			receivedHttpHeader ->httpMethod = POST;
			start = 5;
		}

		start = textCopy(start, text, receivedHttpHeader -> path, ' ') + 1;
		textCopy(start, text, buffer, '\0');
		if(strcmp(buffer, "HTTP/1.0") == 0)
		{
			receivedHttpHeader -> protocol = HTTP1_0;
		}
		else if(strcmp(buffer, "HTTP/1.1") == 0)
		{
			receivedHttpHeader -> protocol = HTTP1_1;
		}
		else if(strcmp(buffer, "HTTP/2") == 0)
		{
			receivedHttpHeader -> protocol = HTTP2;
		}
	}
	else if(startsWith(text, "Host: "))
	{
		textCopy(6, text, receivedHttpHeader -> host, '\0');
	}
}


bool startsWith(const char *str, const char *pre)
{
	size_t lenpre = strlen(pre),
		lenstr = strlen(str);
	return lenstr < lenpre ? false : strncmp(pre, str, lenpre) == 0;
}

int textCopy(int start, char *text, char *dest, char delimiter)
{
	char *ptr = strchr(text + start, delimiter);
	int end = ptr - text;
	memcpy(dest, text + start, end - start);
	return end;
}


