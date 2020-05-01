#include "fyx_esp8266.h"

void clearBuffer_ESP8266(char* espBuffer)
{
	uint16_t i;

	for(i=0;i<ESP8266BUFFER_LENGHT;i++)
		espBuffer[i] = 0;
}
void check_ESP8266(char* espBuffer)
{
	sendString_USART(espUSART,"AT\r\n"); // Standart AT Komutu gonderilir , Cevabi OK olmalidir.
	Delay(1000000L); // 1 saniye gecikme koyuyoruz.
	clearBuffer_ESP8266(espBuffer);
	
	while(strstr(espBuffer,"OK") != 0)
	{
		sendString_USART(espUSART,"AT\r\n"); // Standart AT Komutu gonderilir , Cevabi OK olmalidir.
		clearBuffer_ESP8266(espBuffer);
		Delay(1000000L); // 1 saniye gecikme koyuyoruz.
		sendString_USART(debugUSART,"Modul araniyor...\r\n");
	}
	sendString_USART(debugUSART,"Modul bulundu\r\n");
}

void setClientMode_ESP8266(char* espBuffer)
{
	sendString_USART(espUSART,"AT+CWMODE?\r\n"); // wifi modunu sorguluyoruz
	Delay(1000000L);
	clearBuffer_ESP8266(espBuffer);
	while(strstr(espBuffer,"+CWMODE:1") != 0)
	{
		sendString_USART(espUSART,"AT+CWMODE=1\r\n");
		sendString_USART(espUSART,"AT+RST\r\n");
		Delay(1000000L);
		clearBuffer_ESP8266(espBuffer);
		sendString_USART(debugUSART,"Mod degistiriyor\r\n");
	}
	sendString_USART(debugUSART,"Modul Client Modda\r\n");
}	

void connect_Wifi_ESP8266(char* SSDID,char* passwd,char* espBuffer)
{
	//sendString_USART(espUSART,"AT+CWJAP=\"SUPERONLINE_WiFi_4016\",\"RXUEVVJERWWW\"\r\n");
	sendString_USART(espUSART,"AT+CWJAP=\"");
	sendString_USART(espUSART,SSDID);
	sendString_USART(espUSART,"\",\"");
	sendString_USART(espUSART,passwd);
	sendString_USART(espUSART,"\"\r\n");
	clearBuffer_ESP8266(espBuffer);
	Delay(8000000L);
	while(strstr(espBuffer,"OK") != 0)
	{
		sendString_USART(espUSART,"AT+CWJAP=\"");
		sendString_USART(espUSART,SSDID);
		sendString_USART(espUSART,"\",\"");
		sendString_USART(espUSART,passwd);
		sendString_USART(espUSART,"\"\r\n");
		clearBuffer_ESP8266(espBuffer);
		Delay(8000000L);
		sendString_USART(debugUSART,"MODEME Baglanti Bekleniyor.\r\n");
	}
	
	sendString_USART(debugUSART,"Modeme Baglanti yapildi\r\n");
}

void request_WebSite_ESP8266(char* URLorIP,char* espBuffer)
{
	//sendString_USART(espUSART,"AT+CIPSTART=\"TCP\",\"192.168.1.23\",80\r\n");
	sendString_USART(espUSART,"AT+CIPCLOSE\r\n");
	sendString_USART(debugUSART,"Siteye baglaniliyor...\r\n");
	sendString_USART(espUSART,"AT+CIPSTART=\"TCP\",\"");
	sendString_USART(espUSART,URLorIP);
	sendString_USART(espUSART,"\",80\r\n");
	clearBuffer_ESP8266(espBuffer);

	Delay(1000000L);
	while((strstr(espBuffer,"80 CONNECT")) != 0)
	{
		sendString_USART(debugUSART,"Siteye baglaniliyor...\r\n");
		sendString_USART(espUSART,"AT+CIPSTART=\"TCP\",\"");
		sendString_USART(espUSART,URLorIP);
		sendString_USART(espUSART,"\",80\r\n");
		clearBuffer_ESP8266(espBuffer);
		Delay(1000000L);
	}
	sendString_USART(debugUSART,"Siteye baglanildi\r\n");
	
}

void send_GET_ESP8266(const char* URLorIP,const char* request,char* espBuffer)
{
	int urlSize = 0, requestSize = 0, totalSize = 0;
	char buff[6] = {"\0"};
	urlSize = strlen(URLorIP);
	requestSize = strlen(request);
	totalSize = urlSize + requestSize + 85; // 85 sayisi gelen istek ve url degerine göre degismeden yollanacak byte sayisi
	
	sprintf(buff,"%d",totalSize);
	
	
	sendString_USART(espUSART,"AT+CIPSEND=");
	sendString_USART(espUSART,buff);
	sendString_USART(espUSART,"\r\n");
	clearBuffer_ESP8266(espBuffer);
	Delay(1000000L);
	
	sendString_USART(espUSART,"GET /");
	sendString_USART(espUSART,request);
	sendString_USART(espUSART," HTTP/1.1\r\n");
	sendString_USART(espUSART,"Host: ");
	sendString_USART(espUSART,URLorIP);
	sendString_USART(espUSART,"\r\n");
	sendString_USART(espUSART,"Accept: */*\r\n");
	sendString_USART(espUSART,"Content-Type: text/html\r\n");
	sendString_USART(espUSART,"Content-Length: 0\r\n\r\n\r\n");
	clearBuffer_ESP8266(espBuffer);
	Delay(1000000L);
	
	
	sendString_USART(debugUSART,"Siteden data Alindi");
}
