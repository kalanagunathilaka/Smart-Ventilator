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
#include "string.h"



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


void ADC_Init(int i)
{
    switch(i){
        case 1:{DDRA=0x0;			/* Make ADC port as input */
            ADCSRA = 0x87;			/* Enable ADC, fr/128  */
            ADMUX = 0x00;
            break;	}		/* Vref: Aref, ADC channel: 1 */
        case 2:{DDRA=0x0;			/* Make ADC port as input */
            ADCSRA = 0x87;			/* Enable ADC, fr/128  */
            ADMUX = 0x03;
            break;	}		/* Vref: Aref, ADC channel: 3 */
        case 3:{DDRA=0x0;			/* Make ADC port as input */
            ADCSRA = 0x87;			/* Enable ADC, fr/128  */
            ADMUX = 0x04;
            break;	}		/* Vref: Aref, ADC channel: 4 */

    }


}
int ADC_Read(int channel)
{

    int Ain,AinLow;

    ADMUX=ADMUX|(channel & 0x0f);	/* Set input channel to read */

    ADCSRA |= (1<<ADSC);		/* Start conversion */
    while((ADCSRA&(1<<ADIF))==0);	/* Monitor end of conversion interrupt */

    _delay_us(10);
    AinLow = (int)ADCL;		/* Read lower byte*/
    Ain = (int)ADCH*256;		/* Read higher 2 bits and
					Multiply with weight */
    Ain = Ain + AinLow;
    return(Ain);			/* Return digital value*/
}
const char *concatS(const char *string, char sPercentage[4]);
char *boolstring( _Bool b );
int Average_Blood_Oxygen_level=97;
int Average_Breath_length=50;
int Average_Breath_Per_Min=12;
int Oxygen_percentage=90;
char buff[160];
char status_flag = 0;
char Mobile_no[12];
volatile int buffer_pointer;
unsigned char x;
bool power=true;
bool OxygenAutomation=true;
unsigned long prev_millis0;
unsigned long need_millis0;
int case_num0;
unsigned long prev_millis1;
unsigned long need_millis1;

unsigned long case_num1;
int rBPM;
int rBL;
int rOP;
int i;


int main(void)
{
    /* Replace with your application code */
    DDRC = DDRC | (1<<2); // solenoid valve
    DDRC = DDRC | (1<<3); // solenoid valve
    DDRC = DDRC | (1<<4); // stepper motor
    DDRC = DDRC | (1<<5); // stepper motor
    DDRC = DDRC | (1<<6); // stepper motor
    DDRC = DDRC | (1<<7); // stepper motor
	
	 DDRB=0x8B; // 0,1,3,7--->1 2,4,5,6-->0 for keypad and switch
	 DDRD=DDRD | (0<<2);
	 DDRD=DDRD | (0<<3);
	 DDRD=DDRD | (0<<6);//PD6 as input for power on
	 DDRD=DDRD | (0<<5);//PD5 as Oxygen Automation
	 DDRD=DDRD | (1<<4);//Speaker
	 GICR=1<<INT0 | 1<<INT1 ;
	 MCUCR=0x05;
	
	init_millis(8000000UL);
	sei();
     i2c_init();
	 i2c_start();
	 i2c_write(0x70);
	 lcd_init();
    USART_Init(9600);
    _delay_ms(1000);
    GSMConnect();
	
    
	 //lcd clear funtion
	// while(!=press ok)
	 //	{
	 //	PORTB =0xF0;   //Make all columns 1 and all rows 0.
	 //	if(PINB!=0xF0)
	 //	{
	 //		x=Keypad();
	 //		LCD_Char(x);
	 //	}
	 PORTB = PORTB | (1<<4);
	 PORTB = PORTB | (1<<5);
	 PORTB = PORTB | (1<<6);
	 PORTB = PORTB & (~(1<<0));
	 PORTB = PORTB & (~(1<<1));
	 PORTB = PORTB & (~(1<<3));
	 PORTB = PORTB & (~(1<<7));
	 
			
			//keypad
		
		   
//	 lcd_cmd(0x28);
	// lcd_msg(" Add Phone Num ");
	// _delay_ms(3000);
//	if((PINB&(1<<PINB4))==0|(PINB&(1<<PINB5))==0|(PINB&(1<<PINB6))==0){//this will check if keypad is pressed.
//	 x=Keypad();
//	 lcd_msg(x);
	//		}
	 	
		
	 
    while (1)
    {   
		//for ( i=1;i<4;i++)
		//{
			//adc_init(i);
//
			//if(i==3){
				//rop=adc_read(4);
				//
				//}else if(i==1){
				//rbpm=adc_read(0);
				//}else if(i==2){
				//rbl=adc_read(3);
			//}
		//}
		i=1;

		lcd_cmd(0x28);
		char* rSOP,rSBPM,rSBL;
		itoa(rOP,rSOP,10);
		itoa(rOP,rSBPM,10);
		itoa(rOP,rSBL,10);
		///lcd_msg(rSOP);
		//lcd_msg(rSBPM);
		//lcd_msg(rSBL);

     startOxygenAndAirSupply(60);



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
ISR (INT0_vect) {         //External interrupt
	if((PIND&(0<<PIND2))){
		OxygenAutomation=false;
	}else{
		OxygenAutomation=true;
	}
	
	_delay_ms(50);
	
}
ISR (INT1_vect) {         //External interrupt
	if((PIND&(0<<PIND3))){
		power=false;
		}else{
		power=true;
	}
	
	_delay_ms(50);

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
char *boolstring( _Bool b ) { return b ? "true" : "false"; }
void startOxygenAndAirSupply(int percentage) {
    controlOxygenPercentage(checkBloodOxygenLevel());
	controlSolenoidValve(Oxygen_percentage, Average_Breath_Per_Min);
    startStepperMotor(Average_Breath_Per_Min, Average_Breath_length);
    
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


void openSolenoidValves(double oxygen, double air) {
	air=air*1000;
	oxygen=oxygen*1000;
	
	
	  
	
	if(air>oxygen){
		if(prev_millis1==NULL){
			
			prev_millis1=millis();
			need_millis1=oxygen+prev_millis1;
			case_num1=1;
			PORTC = PORTC | (1<<2);  //open oxygen(normally closed valve)
			PORTC = PORTC & (~(1<<3)); //open air (normally open valve)
			case_num1++;
			}else if(need_millis1<millis()){
			
			switch(case_num1){
				
				case 2:PORTC = PORTC & (~(1<<2));case_num1++;need_millis1=need_millis1+air-oxygen;break; //close oxygen
				case 3:PORTC = PORTC | (1<<3);case_num1++;need_millis1=need_millis1+air;break;//close air
				default:case_num1=NULL;prev_millis1=NULL;
			}
			
			
			
			}else {
			if(prev_millis1==NULL){
				
				prev_millis1=millis();
				need_millis1=air+prev_millis1;
				case_num1=1;
				PORTC = PORTC | (1 << 2); //open oxygen(normally closed valve)
				PORTC = PORTC & (~(1 << 3)); //open air (normally open valve)
				case_num1++;
				}else if(need_millis1<millis()){
				
				switch(case_num1){
					
					case 2:PORTC = PORTC | (1<<3);case_num1++;need_millis1=need_millis1+oxygen-air;break; //close air
					case 3:PORTC = PORTC & (~(1<<2));case_num1++;need_millis1=need_millis1+oxygen;break;//close oxygen
					default:case_num1=NULL;prev_millis1=NULL;
				}
				
			  }

			}

		}
	}
			




double getOxygenTankPressure() {
    return 12000;//return Oxygen tank pressure in pascal
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
   DDRD=DDRD | (1<<4);
   
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
	



	
}

