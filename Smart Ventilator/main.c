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

#include "USART_Interrupt.h"



bool checkStatus();

int oxygenTankPercentage();

void notifyGSM(const char *string, int percentage);

void notifyDisplay();
void delay_ms(int ms);

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

double calculateConstant(double pressure, double density, double inflationTime, double percentage);

double getOxygenTankPressure();

double calculateAnotherTime(double value, double pressure, double density, double percentage);

void openSolenoidValves(double air, double oxygen);
void GSM_Begin();

void GSM_Response();


void sendSMS(char no[14], const char *string);

const char *concatS(const char *string, char sPercentage[4]);

int Average_Blood_Oxygen_level=97;
int Average_Breath_length=50;
int Average_Breath_Per_Min=18;
int Oxygen_percentage=60;
char buff[160];
char status_flag = 0;	
char Mobile_no[14];
volatile int buffer_pointer;   


int main(void)
{
    /* Replace with your application code */
    DDRC = DDRC | (1<<2);
    DDRC = DDRC | (1<<3);

    while (1)
    {
        
        USART_Init(9600);   /* initialize USART */
        _delay_ms(3000);
        GSM_Begin();

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
void GSM_Begin() {
    while(1)
    {

        USART_SendString("ATE0\r"); // send ATE0 to check module is ready or not
        _delay_ms(500);
        if(strstr(buff,"OK"))
        {
            GSM_Response();
            memset(buff,0,160);
            break;
        }
        else
        {
            //print gsm initializing error in display
        }
    }
    _delay_ms(1000);


    USART_SendString("AT+CMGF=1\r"); // select message format as text
    GSM_Response();
    _delay_ms(1000);
}

void GSM_Response() {
    unsigned int timeout=0;
    int CRLF_Found=0;
    char CRLF_buff[2];
    int Response_Length=0;
    while(1)
    {
        if(timeout>=60000)								/*if timeout occur then return */
            return;
        Response_Length = strlen(buff);
        if(Response_Length)
        {
            _delay_ms(2);
            timeout++;
            if(Response_Length==strlen(buff))
            {
                for(int i=0;i<Response_Length;i++)
                {
                    memmove(CRLF_buff,CRLF_buff+1,1);
                    CRLF_buff[1]=buff[i];
                    if(strncmp(CRLF_buff,"\r\n",2))
                    {
                        if(CRLF_Found++==2)				                                    /* search for \r\n in string */
                        {
                            return;
                        }
                    }

                }
                CRLF_Found = 0;

            }

        }
        _delay_ms(1);
        timeout++;
    }
    status_flag=0;
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
    double inflationTime=30/breathPerMin;
    double tAir;//Air Solenoid Valve Time
    double tOxygen;//Oxygen Solenoid Valve Time
    double constValue;
    double Pressure=101325;
    double airDensity=1.225;
    double OxygenDensity=1.355;
    tAir=(1-(oxygenPercentage/100))*0.0005*sqrt(OxygenDensity/(2*Pressure));
    tOxygen=(oxygenPercentage/100)*0.0005*sqrt(airDensity/(2*getOxygenTankPressure()));
    if(tAir>tOxygen){
        tAir=inflationTime;
        constValue= calculateConstant(Pressure, airDensity, inflationTime, (1 - (oxygenPercentage / 100)));
        tOxygen= calculateAnotherTime(constValue, Pressure, airDensity, (1 - (oxygenPercentage / 100)));
        openSolenoidValves(tAir, tOxygen);
    } else{
        tOxygen=inflationTime;

        constValue= calculateConstant(getOxygenTankPressure(), OxygenDensity, inflationTime,oxygenPercentage/100 );
        tAir= calculateAnotherTime(constValue, getOxygenTankPressure(), OxygenDensity, oxygenPercentage / 100);
        openSolenoidValves(tAir, tOxygen);
    }
}
void delay_ms(int ms)
{
    while (ms--) {
        _delay_us(1000);  // one millisecond
    }
}

void openSolenoidValves(double air, double oxygen) {
    while (1){
        if(air>oxygen){
            while(1){
                PORTC = PORTC | (1<<2);  //open oxygen(normally closed valve)
                PORTC = PORTC & (~(1<<3)); //open air (normally open valve)
                delay_ms(air);
                PORTC = PORTC & (~(1<<2));//close oxygen
                delay_ms((air-oxygen));
                PORTC = PORTC | (1<<3);//close air
                delay_ms(air);
            }
        }else {
            while (1) {
                PORTC = PORTC | (1 << 2); //open oxygen(normally closed valve)
                PORTC = PORTC & (~(1 << 3)); //open air (normally open valve)
                delay_ms(air);
                PORTC = PORTC & (~(1 << 2));//close oxygen
                delay_ms(oxygen-air);
                PORTC = PORTC | (1 << 3);//close air
                delay_ms(oxygen);
            }
        }

    }

}

double calculateAnotherTime(double value, double pressure, double density, double percentage) {
    return percentage*value*sqrt(density/(2*pressure));
}

double getOxygenTankPressure() {
    return 0;//return Oxygen tank pressure in pascal
}

double calculateConstant(double pressure, double density, double inflationTime, double percentage) {

    return (inflationTime/percentage)* sqrt(2*pressure/density);
}

void startAirSupply() {
    startStepperMotor(Average_Breath_Per_Min, Average_Breath_length);
    controlSolenoidValve(100, Average_Breath_Per_Min);
}

void startStepperMotor(int breathPerMin, int BreathLength) {


}



int checkBloodOxygenLevel() {

    return 0;//return Blood Oxygen Level
}

int PatientTemp() {
    return 0;//return temperature value
}

void notifySpeaker() {

}

bool checkPatientTemp() {
    if(PatientTemp()>37.2||PatientTemp()<36.1){
        notifyGSM("Temperature Not Normal-",PatientTemp());
        notifyDisplay();
        notifySpeaker();
		return 1;
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
            notifyDisplay();
            return 0;
        } else { return 1; }
    }else{return 0;}
}

void notifyDisplay() {

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




void sendSMS(char no[14], const char *string) {
	char sms_buffer[35];
	buffer_pointer=0;
	sprintf(sms_buffer,"AT+CMGS=\"%s\"\r",no);
	USART_SendString(sms_buffer); //send command AT+CMGS="Mobile No."\r 
	_delay_ms(200);
	while(1)
	{
		if(buff[buffer_pointer]==0x3e) // wait for '>' character
		{
			buffer_pointer = 0;
			memset(buff,0,strlen(buff));
			USART_SendString(string); // send message to given no. 
			USART_TxChar(0x1a); // send Ctrl+Z 
			break;
		}
		buffer_pointer++;
	}
	
}

int oxygenTankPercentage() {
    //return oxygen tank percentage
}


