#ifndef FYX_ESP8266
#define FYX_ESP8266


#include <stm32f10x.h>
#include <string.h>
#include "fyx_init_periph.h"

#define ESP8266BUFFER_LENGHT 500

#define debugUSART	USART2
#define espUSART		USART1

void clearBuffer_ESP8266(char* espBuffer);
void check_ESP8266(char* espBuffer);
void setClientMode_ESP8266(char* espBuffer);
void connect_Wifi_ESP8266(char* SSDID,char* passwd,char* espBuffer);
void request_WebSite_ESP8266(char* URLorIP,char* espBuffer);
void send_GET_ESP8266(const char* URLorIP,const char* request,char* espBuffer);


#endif
