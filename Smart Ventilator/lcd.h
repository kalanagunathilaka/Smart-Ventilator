/*
 * lcd.h
 *
 * Created: 7/5/2022 4:31:08 pm
 *  Author: Shakila Kularathna
 */ 



#ifndef LCD_H_
#define LCD_H_



#include <avr/io.h>
//------------------------//

void lcd_init();			//----LCD Initializing Function
void toggle();				//----Latching function of LCD
void lcd_cmd_hf(char v1);   //----Function to send half byte command to LCD
void lcd_cmd(char v2);		//----Function to send Command to LCD
void lcd_dwr(char v3);		//----Function to send data to LCD
void lcd_msg(char *c);		//----Function to Send String to LCD
void delay(int ms);			//----Delay function
void lcd_lef_sh();			//----Left Shifting Function
void lcd_rig_sh();			//----Right Shifting Function


#endif /* LCD_H_ */