/*
 The blinky demo using an os timer
 TODO : work out why this resets after a while
 */

#include <user_interface.h>
#include <ets_sys.h>
#include <osapi.h>
#include <os_type.h>
#include <gpio.h>
#include <mem.h>
#include <espconn.h>
#include "driver/uart.h"
#include "user_config.h"
#include "httpParser.h"
#include "httpHandler.h"

// see eagle_soc.h for these definitions
#define LED_GPIO 4
#define LED_GPIO_MUX PERIPHS_IO_MUX_GPIO4_U
#define LED_GPIO_FUNC FUNC_GPIO4

#define DELAY 500 /* milliseconds */

LOCAL os_timer_t blink_timer;
LOCAL uint8_t led_state = 0;
LOCAL bool waitingForPost = false;

LOCAL httpHeaderStruct receivedHeader;

//TCP
static struct espconn conn1;
static esp_tcp tcp1;

LOCAL void ICACHE_FLASH_ATTR blink_cb(void *arg) {
	GPIO_OUTPUT_SET(LED_GPIO, led_state);
	led_state ^= 1;
}

void recvCB(void *arg, char *pData, unsigned short len) {
	struct espconn *pEspConn = (struct espconn *) arg;
	//httpHeaderStruct receivedHeader;
	os_printf("Received data!! - length = %d\n", len);
	int i = 0;
	for (i = 0; i < len; i++) {
		os_printf("%c", pData[i]);
	}
	os_printf("\n");
	os_printf("Waiting for post: %d\r\n", waitingForPost);

	if (!waitingForPost) {
		receivedHeader = parseHttp(pData, len);
		os_printf("Method: %d\r\n", receivedHeader.httpMethod);
		switch (receivedHeader.httpMethod) {
		case GET:
			handleGet(pEspConn, &receivedHeader);
			break;
		case POST:
			os_printf("Data length: %d\r\n", receivedHeader.contentLength);
			waitingForPost = true;
			break;
		}
	} else {
		handlePost(pEspConn, &receivedHeader, pData, len);
		waitingForPost = false;
		os_printf("Post handled\r\n");
	}
}

void connectCB(void *arg) {
	struct espconn *pNewEspConn = (struct espconn *) arg;
	espconn_regist_recvcb(pNewEspConn, recvCB);
	os_printf("TCP connection arrived\r\n");
}

void eventHandler(System_Event_t *event) {
	switch (event->event) {
	case EVENT_STAMODE_CONNECTED:
		os_printf("Event: EVENT_STAMODE_CONNECTED\n");
		break;
	case EVENT_STAMODE_DISCONNECTED:
		os_printf("Event: EVENT_STAMODE_DISCONNECTED\n");
		break;
	case EVENT_STAMODE_AUTHMODE_CHANGE:
		os_printf("Event: EVENT_STAMODE_AUTHMODE_CHANGE\n");
		break;
	case EVENT_STAMODE_GOT_IP:
		os_printf("Event: EVENT_STAMODE_GOT_IP\n");
		break;
	case EVENT_SOFTAPMODE_STACONNECTED:
		os_printf("Event: EVENT_SOFTAPMODE_STACONNECTED\n");
		tcp1.local_port = 80;
		conn1.type = ESPCONN_TCP;
		conn1.state = ESPCONN_NONE;
		conn1.proto.tcp = &tcp1;
		espconn_regist_connectcb(&conn1, connectCB);
		espconn_accept(&conn1);
		os_printf("TCP connection handle initialzed\r\n");
		break;
	case EVENT_SOFTAPMODE_STADISCONNECTED:
		os_printf("Event: EVENT_SOFTAPMODE_STADISCONNECTED\n");
		break;
	default:
		//os_printf("Unexpected event: %d\n", event->event);
		break;
	}
}

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
			strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}

void user_rf_pre_init(void) {
}

void user_init(void) {
	// Configure the UART
	uart_init(BIT_RATE_115200, BIT_RATE_74880);
	//ets_uart_printf("\r\nUART initialized.\r\n");
	// Configure pin as a GPIO
	PIN_FUNC_SELECT(LED_GPIO_MUX, LED_GPIO_FUNC);
	// Set up a timer to blink the LED
	// os_timer_disarm(ETSTimer *ptimer)
	//os_timer_disarm(&blink_timer);
	// os_timer_setfn(ETSTimer *ptimer, ETSTimerFunc *pfunction, void *parg)
	//os_timer_setfn(&blink_timer, (os_timer_func_t *) blink_cb, (void *) 0);
	// void os_timer_arm(ETSTimer *ptimer,uint32_t milliseconds, bool repeat_flag)
	//os_timer_arm(&blink_timer, DELAY, 1);
	//ets_uart_printf("Blinky running.\r\n");
	os_printf("UART initialized\r\n");

	wifi_set_event_handler_cb(eventHandler);

	char ssid[32] = SSID;
	char password[64] = SSID_PASSWORD;
	struct softap_config apConfig;

	//Set AP mode
	wifi_set_opmode(STATIONAP_MODE);

	//Set ap settings
	os_memcpy(&apConfig.ssid, ssid, 32);
	os_memcpy(&apConfig.password, password, 64);
	apConfig.channel = 1;
	apConfig.authmode = AUTH_WPA2_PSK;
	wifi_softap_set_config(&apConfig);
}
