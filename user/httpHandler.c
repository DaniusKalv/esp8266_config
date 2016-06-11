/*
 * httpHandler.c
 *
 *  Created on: May 19, 2016
 *      Author: Danius
 */
#include "httpHandler.h"
#include <osapi.h>
#include <user_interface.h>
#include "c_types.h"

espconn *localEspConn;

void handleGet(struct espconn *pEspConn, httpHeaderStruct *header){
	if(strcmp(header->path, "/") == 0)
	{
		uint8 header[] = "HTTP/1.0 200 OK\r\n\r\n"; //"HTTP/1.0 200 OK\r\nContent-Encoding:gzip\r\n\r\n";
		os_printf("Buffer size: %d\r\n", sizeof(header)+HTML_SIZE-1);
		uint8 buffer[sizeof(header)+HTML_SIZE-1];
		memcpy(buffer, header, sizeof(header)-1);
		readFlashUnaligned(buffer + sizeof(header)-1, (char *)HTML_POS, HTML_SIZE);
		os_printf("Read flash\r\n");
		espconn_send(pEspConn, buffer, (uint16)HTML_SIZE+sizeof(header)-1);
		os_printf("Disconnecting...\r\n");
		espconn_disconnect(pEspConn);
	}
	else if(strcmp(header->path, "/scanWifi")){
		localEspConn = pEspConn;
		wifi_station_scan(NULL, scanCB);
	}
	else
	{
		char data[] = "HTTP/1.0 404 Not Found\r\nContent-Type: text/html\r\n\r\n<HTML><HEAD><TITLE>ESP8266</TITLE><link rel='icon' href='data:;base64,iVBORw0KGgo='></HEAD><BODY><H1>Shit man this page is not here</H1><p>I'm just a small ESP8266 module and I have to handle this error page</BODY></HTML>\r\n";
		int length = sizeof(data);
		espconn_send(pEspConn, data, length);
		espconn_disconnect(pEspConn);
	}
}

void readFlashUnaligned(char *dst, char *src, int len) {
	uint8_t src_offset = ((uint32_t) src) & 3;
	uint32_t src_address = ((uint32_t) src) - src_offset;

	uint32_t tmp_buf[len / 4 + 2];
	spi_flash_read((uint32) src_address, (uint32*) tmp_buf, len + src_offset);
	memcpy(dst, ((uint8_t*) tmp_buf) + src_offset, len);
}

void scanCB(void *arg, STATUS status) {
	struct bss_info *bssInfo;
	bssInfo = (struct bss_info *) arg;
	// skip the first in the chain … it is invalid
	bssInfo = STAILQ_NEXT(bssInfo, next);

	char buffer[300] = "HTTP/1.0 200 OK\r\n\r\n{\"ssids\":[";
	//os_printf("%s\r\n", buffer);
	uint16 i = strlen(buffer);
	//os_printf("Length: %d\r\n", i);

	bool first = true;
	while (bssInfo != NULL) {
		//os_printf("ssid: %s\n", bssInfo->ssid);
		if(first){
			first = !first;
		}
		else{
			buffer[i] = ',';
			i++;
		}
		buffer[i] = '"';
		i++;
		memcpy(buffer + i, bssInfo->ssid, strlen(bssInfo->ssid));
		i += strlen(bssInfo->ssid);
		buffer[i] = '"';
		i++;
		//os_printf("%s\r\n", buffer);
		bssInfo = STAILQ_NEXT(bssInfo, next);
	}
	memcpy(buffer + i, "]}", 2);
	//os_printf("Buffer length: %d\r\n", strlen(buffer));
	espconn_send(localEspConn, buffer, strlen(buffer));
	espconn_disconnect(localEspConn);
}
