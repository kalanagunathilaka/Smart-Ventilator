/*
 * GSM.h
 *
 * Created: 5/22/2022 5:37:53 PM
 *  Author: Yesitha Sathsara
 */ 


#ifndef GSM_H_
#define GSM_H_

#define ISP "mobitel"
#define APN "dialog"
#include "USART.h"
void sendSMS(char no[], const char *string) {
	USART_SendString("AT\r");
	_delay_ms(10);
	USART_SendString("AT+CMGF=1\r");
	_delay_ms(10);
	char data_buffer[100];
	sprintf(data_buffer,"AT+CMGS=%s\r",no);
	USART_SendString(data_buffer);
	memset(USART_BUFFER, 0, 100);
	sprintf(data_buffer,"%s\r",string);
	USART_SendString(data_buffer);
	memset(USART_BUFFER, 0, 100);//clear data buffer

}
void GSMConnect(){
	USART_SendString("ATE0\r");
	_delay_ms(10);
	char data_buffer[100];
	sprintf(data_buffer,"AT+CSTT=\"%s\",\"%s\",\"%s\"\r",APN,ISP,ISP);
	USART_SendString(data_buffer);
	memset(USART_BUFFER, 0, 100);
	_delay_ms(10);
	USART_SendString("AT+SAPBR=1,1\r");
}





#endif /* GSM_H_ */