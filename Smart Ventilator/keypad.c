/*
 * GccApplication2.c
 *
 * Created: 5/12/2022 11:27:58 PM
 * Author : Piyumanthi
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include<util/delay.h>



char Keypad()
{
	
//	PORTB=0b1111110;         // make first row 0
	PORTB = PORTB | (1<<4);
	PORTB = PORTB | (1<<5);
	PORTB = PORTB | (1<<6);
	PORTB = PORTB & (~(1<<0));
	PORTB = PORTB | (1<<1);
	PORTB = PORTB | (1<<3);
	PORTB = PORTB | (1<<7); //<--2
	if((PINB &(1<<PINB4))==0)
	{
		_delay_ms(3);
		return"1";
	}

	else if((PINB&(1<<PINB5))==0)
	{
		_delay_ms(3);
		return"2";
	}
	else if((PINB&(1<<PINB6))==0)
	{
		_delay_ms(3);
		return"3";
	}
	

//	PORTB=0b1111101;   // make second row 0
		PORTB = PORTB | (1<<4);
		PORTB = PORTB | (1<<5);
		PORTB = PORTB | (1<<6);
		PORTB = PORTB & (~(1<<1));
		PORTB = PORTB | (1<<0);
		PORTB = PORTB | (1<<3);
		PORTB = PORTB | (1<<7);// <--2
	if((PINB &(1<<PINB4))==0)
	{
		_delay_ms(3);
		return"4";
	}
	else if((PINB&(1<<PINB5))==0)
	{
		_delay_ms(3);
		return"5";
	}
	else if((PINB&(1<<PINB6))==0)
	{
		_delay_ms(3);
		return"6";
	}
	

//	PORTB=0b1111011;   // make third row 0
	PORTB = PORTB | (1<<4);
	PORTB = PORTB | (1<<5);
	PORTB = PORTB | (1<<6);
	PORTB = PORTB & (~(1<<7));
	PORTB = PORTB | (1<<1);
	PORTB = PORTB | (1<<3);
	PORTB = PORTB | (1<<0); //<--2
	if((PINB &(1<<PINB4))==0)
	{
		_delay_ms(3);
		return"7";
	}
	else if((PINB&(1<<PINB5))==0)
	{
		_delay_ms(3);
		return"8";
	}
	else if((PINB&(1<<PINB6))==0)
	{
		_delay_ms(3);
		return"9";
	}
	

	//PORTB=0b1110111;   // make fourth row 0
		PORTB = PORTB | (1<<4);
		PORTB = PORTB | (1<<5);
		PORTB = PORTB | (1<<6);
		PORTB = PORTB & (~(1<<3));
		PORTB = PORTB | (1<<0);
		PORTB = PORTB | (1<<1);
		PORTB = PORTB | (1<<7);// <--2
	if((PINB &(1<<PINB4))==0)
	{
		_delay_ms(3);
		
		return"CLR";
	}
	else if((PINB&(1<<PINB5))==0)
	{
		_delay_ms(3);
		return"0";
	}
	else if((PINB&(1<<PINB6))==0)
	{
		_delay_ms(3);
		return"OK";
	}
	

}

	
	
	//while(1)
	//	{
	//		PORTB =0xF0;   //Make all columns 1 and all rows 0.
	//		if(PINB!=0xF0)
	//		{
	//			x=Keypad();
	//			lcd_msg(x);
	//		}
	
	//	}
	//	return 0;

