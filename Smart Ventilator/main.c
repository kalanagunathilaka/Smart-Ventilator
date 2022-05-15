/*
 * Smart Ventilator.c
 *
 * Created: 11/27/2021 1:26:16 PM
 * Author : Yesitha Sathsara
 */
#define F_CPU 8000000UL
#include <avr/io.h>
#include <stdbool.h>
#include <math.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <millis.h>
#include <avr/interrupt.h>
#include "lcd.h"
#include "i2c.h"
#include "keypad.h"



#define ISP "mobitel"
#define APN "dialog"
#include "USART.h"

bool checkStatus();

int oxygenTankPercentage();

void notifyGSM(const char *string, int percentage);

void notifyDisplay(const char *string);
void delay_ms(double ms);

bool automationOn();

void getParametersFromKnobs();

bool checkPatientTemp();

int PatientTemp();

int checkBloodOxygenLevel();



void startAirSupply();
void controlOxygenPercentage(int bloodOxygenLevel);

void notifySpeaker();

bool turnOn();

void startStepperMotor(int breathPerMin, int BreathLength);

void startOxygenAndAirSupply(int percentage);

void controlSolenoidValve(double oxygenPercentage, int breathPerMin);
void rotateFullBackward(int breathPerMin);
void rotateFullForward(int breathPerMin);




double getOxygenTankPressure();



void openSolenoidValves(double air, double oxygen);
void GSMConnect();




void sendSMS(char no[], const char *string);

const char *concatS(const char *string, char sPercentage[4]);

int Average_Blood_Oxygen_level=97;
int Average_Breath_length=50;
int Average_Breath_Per_Min=12;
int Oxygen_percentage=90;
char buff[160];
char status_flag = 0;
char Mobile_no[12];
volatile int buffer_pointer;
unsigned char x;
bool power;
bool OxygenAutomation;
unsigned long prev_millis0;
unsigned long need_millis0;
int case_num0;
unsigned long prev_millis1;
unsigned long case_num1;


int main(void)
{
    /* Replace with your application code */
    DDRC = DDRC | (1<<2); // solenoid valve
    DDRC = DDRC | (1<<3); // solenoid valve
    DDRC = DDRC | (1<<4); // stepper motor
    DDRC = DDRC | (1<<5); // stepper motor
    DDRC = DDRC | (1<<6); // stepper motor
    DDRC = DDRC | (1<<7); // stepper motor
	DDRB=0x0F;            //Make PB0 to PB3 = output and PB4 to PB6=input for key pad
	init_millis(8000000UL);
	sei();
     i2c_init();
	 i2c_start();
	 i2c_write(0x70);
	 lcd_init();
    USART_Init(9600);
    _delay_ms(1000);
    GSMConnect();
	
     lcd_cmd(0x80);
     lcd_msg("Enter Number");
	 //lcd clear funtion
	// while(!=press ok)
	 //	{
	 //	PORTB =0xF0;   //Make all columns 1 and all rows 0.
	 //	if(PINB!=0xF0)
	 //	{
	 //		x=Keypad();
	 //		LCD_Char(x);
	 //	}
	 
	 
    while (1)
    {   
		
     startStepperMotor(12,12);



        if(checkStatus()){
            if(automationOn()){
                if(checkPatientTemp()){

                    if(checkBloodOxygenLevel() < Average_Blood_Oxygen_level){
                        startOxygenAndAirSupply(Oxygen_percentage);
                    }else{
                        startAirSupply();
                    }
                }
            }else{
                checkPatientTemp();
                getParametersFromKnobs();
                startOxygenAndAirSupply(Oxygen_percentage);
            }
        }else{return 0;}
    }
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

void startOxygenAndAirSupply(int percentage) {
    controlOxygenPercentage(checkBloodOxygenLevel());
    startStepperMotor(Average_Breath_Per_Min, Average_Breath_length);
    controlSolenoidValve(Oxygen_percentage, Average_Breath_Per_Min);
}
void controlOxygenPercentage(int bloodOxygenLevel){
    //update variable Oxygen Percentage According to Blood Oxygen Level
}

void controlSolenoidValve(double oxygenPercentage, int breathPerMin) {
    double inflationTime=30.0000/breathPerMin;
    double tAir;//Air Solenoid Valve Time
    double tOxygen;//Oxygen Solenoid Valve Time
    double constValue;
    double Pressure=101325;
    double airDensity=1.225;
    double OxygenDensity=1.355;


   constValue=(0.79*(1.266*oxygenPercentage-26.67)/(100-oxygenPercentage))*sqrt(Pressure*OxygenDensity/getOxygenTankPressure()/airDensity);
   if(constValue>1){
       tOxygen=inflationTime;
       tAir=tOxygen/constValue;
       openSolenoidValves(tAir,tOxygen);// values in s
   } else{
       tAir=inflationTime;
       tOxygen=tAir*constValue;
       openSolenoidValves(tAir,tOxygen);//values in s
   }
}
void delay_ms(double ms)
{ 
	int waitTime=ms*1000;
	
	for(int i=waitTime;i>0;i--) {
	_delay_us(1);  // one microsecond
	}
 //init_millis(8000000UL); //frequency the atmega328p is running at
 //sei();
 //unsigned long prev_millis0; //the last time the led was toggled
// prev_millis0 = millis();
 
// for(;;)
// {
//	 if (millis() - prev_millis0 > ms)
	// {
		 
	//	 prev_millis0 = millis();
	//     return;
	// }
//}


	
}

void openSolenoidValves(double air, double oxygen) {
    air=air*1000;
	oxygen=oxygen*1000;
		
        if(air>oxygen){
            
				
                PORTC = PORTC | (1<<2);  //open oxygen(normally closed valve)
                PORTC = PORTC & (~(1<<3)); //open air (normally open valve)
                delay_ms(oxygen);
                PORTC = PORTC & (~(1<<2));//close oxygen
                delay_ms(air-oxygen);
                PORTC = PORTC | (1<<3);//close air
                delay_ms(air);
            
        }else {
            
                PORTC = PORTC | (1 << 2); //open oxygen(normally closed valve)
                PORTC = PORTC & (~(1 << 3)); //open air (normally open valve)
                delay_ms(air);
                PORTC = PORTC & (~(1 << 2));//close oxygen
                delay_ms(oxygen-air);
                PORTC = PORTC | (1 << 3);//close air
                delay_ms(oxygen);
            
        }

    

}



double getOxygenTankPressure() {
    return 0;//return Oxygen tank pressure in pascal
}



void startAirSupply() {
    startStepperMotor(Average_Breath_Per_Min, Average_Breath_length);
    controlSolenoidValve(100, Average_Breath_Per_Min);
}





int checkBloodOxygenLevel() {

    return 0;//return Blood Oxygen Level
}

int PatientTemp() {
    return 37;//return temperature value
}

void notifySpeaker() {

}

bool checkPatientTemp() {
    if(PatientTemp()>37.2||PatientTemp()<36.1){
        notifyGSM("Temperature Not Normal-",PatientTemp());
		char Spercentage[4];
		itoa(PatientTemp(),Spercentage,10);//convert int to string
		notifyDisplay(concatS("Temperature Not Normal-", Spercentage)); 
        notifySpeaker();
        return 0;
    }else{
        return 1;
    }
    //if normal return 1 else notify speaker and gsm
}

void getParametersFromKnobs() {

    //get values and update  Breath per min,Oxygen Percentage,Breath Length
}

bool automationOn() {
    return 1;//Check Automation On/Off
}

bool turnOn() {
    return 1;//return 1 if power on switched pressed else return 0
}

bool checkStatus() {
    if (turnOn()) {
        if (oxygenTankPercentage() < 10) {
            notifyGSM("Oxygen Tank Percentage : ", oxygenTankPercentage());
			char Spercentage[4];
			itoa(oxygenTankPercentage(),Spercentage,10);//convert int to string
            notifyDisplay(concatS("Oxygen Tank Percentage ", Spercentage));
            return 0;
        } else { return 1; }
    }else{return 0;}
}

void notifyDisplay(const char *string) {
   lcd_msg(string);
}

void notifyGSM(const char *string, int percentage) {
    char Spercentage[4];
    itoa(percentage,Spercentage,10);//convert int to string


    sendSMS(Mobile_no, concatS(string,Spercentage));
}
const char *concatS(const char *string, char sPercentage[4]) {
    char *result = malloc(strlen(string) + strlen(sPercentage) + 1);
    strcpy(result, string);
    strcat(result, sPercentage);
    return result;
}




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

int oxygenTankPercentage() {
    return 80;
}
void startStepperMotor(int breathPerMin, int BreathLength) {
	if(prev_millis0==NULL){
		
		prev_millis0=millis();
		need_millis0=30000/(breathPerMin*10)+prev_millis0;
		case_num0=1;
		PORTC = PORTC | (1<<4);case_num0++;
		}else if(need_millis0<millis()){
		
		switch(case_num0){
			
			case 2:PORTC = PORTC | (1<<6);case_num0++;need_millis0=need_millis0+30000/(breathPerMin*10);break;
			case 3:PORTC = PORTC & (~(1<<4));case_num0++;need_millis0=need_millis0+30000/(breathPerMin*10);break;
			case 4:PORTC = PORTC | (1<<5);case_num0++;need_millis0=need_millis0+30000/(breathPerMin*10);break;
			case 5:PORTC = PORTC & (~(1<<6));case_num0++;need_millis0=need_millis0+30000/(breathPerMin*10);break;
			case 6:PORTC = PORTC | (1<<7);case_num0++;need_millis0=need_millis0+30000/(breathPerMin*10);break;
			case 7:PORTC = PORTC & (~(1<<5));case_num0++;need_millis0=need_millis0+30000/(breathPerMin*10);break;
			case 8:PORTC = PORTC | (1<<4);case_num0++;need_millis0=need_millis0+30000/(breathPerMin*10);break;
			case 9:PORTC = PORTC & (~(1<<7));case_num0++;need_millis0=need_millis0+30000/(breathPerMin*10);break;
			case 10:PORTC = PORTC & (~(1<<4));case_num0++;need_millis0=need_millis0+30000/(breathPerMin*10);break;
			case 11:PORTC = PORTC | (1<<4);case_num0++;need_millis0=need_millis0+30000/(breathPerMin*10);break;
			case 12:PORTC = PORTC | (1<<7);case_num0++;need_millis0=need_millis0+30000/(breathPerMin*10);break;
			case 13:PORTC = PORTC & (~(1<<4));case_num0++;need_millis0=need_millis0+30000/(breathPerMin*10);break;
			case 14:PORTC = PORTC | (1<<5);case_num0++;need_millis0=need_millis0+30000/(breathPerMin*10);break;
			case 16:PORTC = PORTC & (~(1<<7));case_num0++;need_millis0=need_millis0+30000/(breathPerMin*10);break;
			case 17:PORTC = PORTC | (1<<6);case_num0++;need_millis0=need_millis0+30000/(breathPerMin*10);break;
			case 18:PORTC = PORTC & (~(1<<5));case_num0++;need_millis0=need_millis0+30000/(breathPerMin*10);break;
			case 19:PORTC = PORTC & (~(1<<6));case_num0++;need_millis0=need_millis0+30000/(breathPerMin*10);break;
			case 20:PORTC = PORTC & (~(1<<4));case_num0++;need_millis0=need_millis0+30000/(breathPerMin*10);break;
			default:case_num0=NULL;prev_millis0=NULL;
		}
	}
	//rotateFullForward(breathPerMin);
		
	//rotateFullBackward(breathPerMin);
	
}

void rotateFullForward(int breathPerMin){
	if(prev_millis1==NULL){
		
		prev_millis1=millis();
		case_num0=30000/(breathPerMin*10)+prev_millis1;
		case_num0=1;
		PORTC = PORTC | (1<<4);case_num0++;
		}else if(case_num0<millis()){
		
		switch(case_num0){
			
			case 2:PORTC = PORTC | (1<<4);case_num0++;case_num0=case_num0+30000/(breathPerMin*10);break;
			case 3:PORTC = PORTC | (1<<7);case_num0++;case_num0=case_num0+30000/(breathPerMin*10);break;
			case 4:PORTC = PORTC & (~(1<<4));case_num0++;case_num0=case_num0+30000/(breathPerMin*10);break;
			case 5:PORTC = PORTC | (1<<5);case_num0++;case_num0=case_num0+30000/(breathPerMin*10);break;
			case 6:PORTC = PORTC & (~(1<<7));case_num0++;case_num0=case_num0+30000/(breathPerMin*10);break;
			case 7:PORTC = PORTC | (1<<6);case_num0++;case_num0=case_num0+30000/(breathPerMin*10);break;
			case 8:PORTC = PORTC & (~(1<<5));case_num0++;case_num0=case_num0+30000/(breathPerMin*10);break;
			case 9:PORTC = PORTC & (~(1<<6));case_num0++;case_num0=case_num0+30000/(breathPerMin*10);break;
			case 10:PORTC = PORTC & (~(1<<4));case_num0++;case_num0=case_num0+30000/(breathPerMin*10);break;
			default:case_num0=NULL;prev_millis1=NULL;
		}
	}
		
	
	
	
}

void rotateFullBackward(int breathPerMin){
	
	if(prev_millis0==NULL){
		
		prev_millis0=millis();
		need_millis0=30000/(breathPerMin*10)+prev_millis0;
		case_num0=1;
		PORTC = PORTC | (1<<4);case_num0++;
		}else if(need_millis0<millis()){
		
		switch(case_num0){
			
			case 2:PORTC = PORTC | (1<<6);case_num0++;need_millis0=need_millis0+30000/(breathPerMin*10);break;
			case 3:PORTC = PORTC & (~(1<<4));case_num0++;need_millis0=need_millis0+30000/(breathPerMin*10);break;
			case 4:PORTC = PORTC | (1<<5);case_num0++;need_millis0=need_millis0+30000/(breathPerMin*10);break;
			case 5:PORTC = PORTC & (~(1<<6));case_num0++;need_millis0=need_millis0+30000/(breathPerMin*10);break;
			case 6:PORTC = PORTC | (1<<7);case_num0++;need_millis0=need_millis0+30000/(breathPerMin*10);break;
			case 7:PORTC = PORTC & (~(1<<5));case_num0++;need_millis0=need_millis0+30000/(breathPerMin*10);break;
			case 8:PORTC = PORTC | (1<<4);case_num0++;need_millis0=need_millis0+30000/(breathPerMin*10);break;
			case 9:PORTC = PORTC & (~(1<<7));case_num0++;need_millis0=need_millis0+30000/(breathPerMin*10);break;
			case 10:PORTC = PORTC & (~(1<<4));case_num0++;need_millis0=need_millis0+30000/(breathPerMin*10);break;
			default:case_num0=NULL;prev_millis0=NULL;
		}
	}
	
	//delay_ms(30000/(breathPerMin*10));
	
	
	//delay_ms(30000/(breathPerMin*10));
	
	
	//delay_ms(30000/(breathPerMin*10));
	
	
	//delay_ms(30000/(breathPerMin*10));
	
	
	//delay_ms(30000/(breathPerMin*10));
	
	
	//delay_ms(30000/(breathPerMin*10));
	
	
	//delay_ms(30000/(breathPerMin*10));
	
	
	//delay_ms(30000/(breathPerMin*10));
	
	
	//delay_ms(30000/(breathPerMin*10));
	
	
	//delay_ms(30000/(breathPerMin*10));
	
}

