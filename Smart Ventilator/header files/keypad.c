/*
 * GccApplication2.c
 *
 * Created: 5/12/2022 11:27:58 PM
 * Author : Piyumanthi
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include<util/delay.h>
#include "LCD16x2_4bit.h"

char Keypad()
{
	
	PORTB=0b1111110;         // make first row 0
	if((PINB &(1<<PINB4))==0)
	{
		_delay_ms(3);
		return'1';
	}

	else if((PINB&(1<<PINB5))==0)
	{
		_delay_ms(3);
		return'2';
	}
	else if((PINB&(1<<PINB6))==0)
	{
		_delay_ms(3);
		return'3';
	}
	

	PORTB=0b1111101;   // make second row 0
	if((PINB &(1<<PINB4))==0)
	{
		_delay_ms(3);
		return'4';
	}
	else if((PINB&(1<<PINB5))==0)
	{
		_delay_ms(3);
		return'5';
	}
	else if((PINB&(1<<PINB6))==0)
	{
		_delay_ms(3);
		return'6';
	}
	

	PORTB=0b1111011;   // make third row 0
	if((PINB &(1<<PINB4))==0)
	{
		_delay_ms(3);
		return'7';
	}
	else if((PINB&(1<<PINB5))==0)
	{
		_delay_ms(3);
		return'8';
	}
	else if((PINB&(1<<PINB6))==0)
	{
		_delay_ms(3);
		return'9';
	}
	

	PORTB=0b1110111;   // make fourth row 0
	if((PINB &(1<<PINB4))==0)
	{
		_delay_ms(3);
		return'*';
	}
	else if((PINB&(1<<PINB5))==0)
	{
		_delay_ms(3);
		return'0';
	}
	else if((PINB&(1<<PINB6))==0)
	{
		_delay_ms(3);
		return'#';
	}
	

}

int main(void)
{
	unsigned char x;
	DDRB=0x0F;         //Make PB0 to PB3 = output and PB4 to PB6=input
	LCD_Init ();
	while(1)
	{
		PORTB =0xF0;   //Make all columns 1 and all rows 0.
		if(PINB!=0xF0)
		{
			x=Keypad();
			LCD_Char(x);
		}
		
	}
	return 0;
}

