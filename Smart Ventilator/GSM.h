/*
 * GSM.h
 *
 * Created: 5/22/2022 5:37:53 PM
 *  Author: Yesitha Sathsara
 */ 


#ifndef GSM_H_
#define GSM_H_


#include <string.h>
#include "USART_Interrupt.h"				// include USART Header file 
#include "lcd.h"
#define ISP "dialog"
#define APN "dialog"
char data_buffer[100];
char buff[160];								// buffer to store responses and messages 
char status_flag = 0;	
volatile int buffer_pointer;

void sendSMS(char no[11], const char *string) {
	
	USART_SendString("AT+CMGF=1\r"); // Set SMS system into text mode
	_delay_ms(10);
	
	sprintf(data_buffer,"AT+CMGS=%s\r",no);// send command AT+CMGS="Mobile No."
	USART_SendString(data_buffer);
	_delay_ms(200);
	while(1)
	{
		if(buff[buffer_pointer]==0x3e)                  //wait for '>' character
		{
			buffer_pointer = 0;
			memset(buff,0,strlen(buff));				// emptying "buff" 
			USART_SendString(string);                      // send msg 
			USART_TxChar(0x1a);                         //send Ctrl+Z then only message will transmit
			break;
		}
		buffer_pointer++;
	}
	_delay_ms(300);
	buffer_pointer = 0;
	memset(buff,0,strlen(buff));				// emptying "buff" 
	memset(data_buffer,0,strlen(data_buffer));	// emptying "sms_buffer" 

}
void GSMConnect(){
	while(1){
		USART_SendString("AT\r");// send 'ATE0' to check module is ready or not
		_delay_ms(500);
		if(strstr(buff,"OK"))
		{
			sprintf(data_buffer,"AT+CSTT=\"%s\",\"%s\",\"%s\"\r",APN,ISP,ISP); //Set APN,User name,Password
			USART_SendString(data_buffer);
			memset(data_buffer, 0, 100);              
			_delay_ms(10);
			break;
			
		}
		else
		{
			lcd_msg("GSM Error!");
		}
	}
	
		_delay_ms(1000);
		lcd_cmd(0x01);
	}
	
    ISR(USART_RXC_vect)
    {
	    buff[buffer_pointer] = UDR;							// copy UDR(received value) to buffer 
	    buffer_pointer++;
	    status_flag = 1;						            // flag for new message arrival 
    }

	

	







#endif /* GSM_H_ */