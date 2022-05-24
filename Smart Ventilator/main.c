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
#include "GSM.h"
#include "solenoidValves.h"
#include "stepper motor.h"
#include "potentiometer.h"

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
bool checkPower();
int checkPatientExpPresure();

const char *concatS(const char *string, char sPercentage[4]);
char *boolstring(_Bool b);
int Average_Blood_Oxygen_level = 94;
int Average_Breath_length = 50;
int Average_Breath_Per_Min = 12;
int Oxygen_percentage = 90;
char buff[160];
char status_flag = 0;
char Mobile_no[11];
volatile int buffer_pointer;
unsigned char x;
bool power = true;
bool OxygenAutomation = true;

int rBPM;
int rBL;
int rOP;
int i;
int caseADC = 1;
char String[5];
int value;
int powerfailure;
unsigned long case_num2;
unsigned long prev_millis2;
unsigned long need_millis2;

int main(void)
{

    DDRC = DDRC | (1 << 2); // solenoid valve
    DDRC = DDRC | (1 << 3); // solenoid valve
    DDRC = DDRC | (1 << 4); // stepper motor
    DDRC = DDRC | (1 << 5); // stepper motor
    DDRC = DDRC | (1 << 6); // stepper motor
    DDRC = DDRC | (1 << 7); // stepper motor

    DDRB = 0x0F;            // 0,1,3,2--->1   4,5,6,7-->0 for keypad
    DDRD = DDRD | (0 << 2); // PD2 as Oxygen Automation
    DDRD = DDRD | (0 << 3); // PD3 as input for power on
    DDRD = DDRD | (1 << 4); // Speaker
    DDRA = DDRA | (0 << 7); // PA7 as input for power supply

    MCUCR |= 1 << ISC01; // interrupt fire on falling edge in INT0
    MCUCR |= 1 << ISC11; // interrupt fire on falling edge in INT1

    //
    GICR |= 1 << INT0; // Enable the external interrupt source in general interrupt control register
    GICR |= 1 << INT1; // Enable the external interrupt source in general interrupt control register

    init_millis(8000000UL);
    sei();
    ADC_Init();
    i2c_init();
    i2c_start();
    i2c_write(0x70);
    lcd_init();

    lcd_cmd(0x80);
    lcd_msg("Enter Phone");
    _delay_ms(30);
    lcd_cmd(0xC0);
    _delay_ms(10);
    lcd_msg("Number");
    _delay_ms(30);
    lcd_cmd(0x01);
    _delay_ms(100);
    lcd_cmd(0x80);

initialPoint:
    lcd_cmd(0x01);

    PORTB = 0xF0;
    int f = 0;
    do
    {
        x = Keypad();

        Mobile_no[f] = x;
        if (((PINB & (1 << PINB4)) == 0) || ((PINB & (1 << PINB5)) == 0) || ((PINB & (1 << PINB6)) == 0))
        {
            f++;
        }
        lcd_msg(x);
        _delay_ms(70);

        if ((PINB & (1 << PINB4)) == 0) // clr button
        {
            goto initialPoint;
        }
        if ((PINB & (1 << PINB6)) == 0) // ok button
        {
            break;
        }

    } while (PINB != 0xF0);

    lcd_cmd(0x01);
    lcd_cmd(0x80);
    lcd_msg(Mobile_no);
    for (int i = 0; i < 11; i++)
    {

        lcd_msg(Mobile_no[i]);
        _delay_ms(50);
    }
    _delay_ms(100);
    lcd_cmd(0x01);

    lcd_cmd(0x80);
    lcd_msg("Number");
    lcd_cmd(0xc0);
    lcd_msg("Registered");
    _delay_ms(100);
    lcd_cmd(0x01);

    USART_Init(9600);
    _delay_ms(1000);
    GSMConnect();
    sendSMS(Mobile_no, "test");

    while (1)
    {
		
		if(prev_millis2==NULL){
			
			prev_millis2=millis();
			need_millis2=500+prev_millis2;
			case_num2=1;
			     char Spercentage[4];
			     lcd_cmd(0x01);
				 lcd_cmd(0x80);
				 itoa(checkBloodOxygenLevel(), Spercentage, 10);
				 lcd_msg(concatS("PBOxyL-", Spercentage));
				 itoa(rBPM, Spercentage, 10);
				 lcd_msg(concatS("HR-", Spercentage));
				 lcd_cmd(0xc0);
				 itoa(checkPatientTemp(), Spercentage, 10);
				 lcd_msg(concatS("Temp-",Spercentage));
				 itoa(checkPatientExpPresure(), Spercentage, 10);
				 lcd_msg(concatS("ExpP-", Spercentage));
			case_num2++;
			}else if(need_millis1<millis()){
			
			switch(case_num2){
				
				case 2:{
					lcd_cmd(0x01);
					lcd_cmd(0x80);
					char Spercentage1[4];
					itoa(rBPM, Spercentage1, 10);
					lcd_msg(concatS("BPM-", Spercentage1));
					itoa(rBL, Spercentage1, 10);
					lcd_msg(concatS("BL-", Spercentage1));
					lcd_cmd(0xc0);
					itoa(getOxygenTankPressure(), Spercentage1, 10);
					lcd_msg(concatS("OxyTankP-", Spercentage1));
					itoa(Oxygen_percentage, Spercentage1, 10);
					lcd_msg(concatS("AOxyP-", Spercentage1));
					case_num2++;
					need_millis2=need_millis2+500;
					break;
				} 
				
				default:{case_num2=NULL;
					prev_millis2=NULL;
				}
			}
		}
		
		
		
		
		
		
		
       
       

        switch (caseADC)
        {
        case 1:
        {
            value = ADC_Read(0);
            rBPM = ((value * 14) / 1024) + 10; // BPM range vary from 10 to 24
            caseADC++;
            ADC_Init();
            break;
        }
        case 2:
        {
            value = ADC_Read(3);
            rBL = ((value * 650) / 1024) + 250; // BL range vary from 250 to 900
            caseADC++;
            ADC_Init();
            break;
        }
        case 3:
        {
            value = ADC_Read(4);
            rOP = (100 * value) / 1024; // OP range vary from 0 to 100
            caseADC = 1;
            ADC_Init();
            break;
        }
        }

        if (checkStatus())
        {
            if (automationOn())
            {
                if (checkPatientTemp())
                {

                    if (checkBloodOxygenLevel() < Average_Blood_Oxygen_level)
                    {
                        startOxygenAndAirSupply(Oxygen_percentage);
                    }
                    else
                    {
                        startAirSupply();
                    }
                }
            }
            else
            {
                checkPatientTemp();
                getParametersFromKnobs(); // update o2 percentage manually
                startOxygenAndAirSupply(Oxygen_percentage);
            }
        }
        else
        {
            return 0;
        }
    }
}
ISR(INT0_vect)
{ // External interrupt

    OxygenAutomation = !OxygenAutomation;

    if (OxygenAutomation)
    {
        lcd_cmd(0x01);
        lcd_msg("O2 Auto On");
    }
    else
    {
        lcd_cmd(0x01);
        lcd_msg("O2 Auto Off");
    }
    _delay_ms(50);
    lcd_cmd(0x01);
}
ISR(INT1_vect)
{ // External interrupt
    power = !power;
    if (power)
    {
        lcd_cmd(0x01);
        lcd_msg("Power On");
    }
    else
    {
        lcd_cmd(0x01);
        lcd_msg("Power Off");
    }
    _delay_ms(50);
    lcd_cmd(0x01);
}

void startOxygenAndAirSupply(int percentage)
{
    controlOxygenPercentage(checkBloodOxygenLevel());
    controlSolenoidValve(Oxygen_percentage, rBPM);
    startStepperMotor(rBPM, rBL);
}
void controlOxygenPercentage(int bloodOxygenLevel)
{
    if (bloodOxygenLevel < 40)
    {
        Oxygen_percentage = 90;
    }
    else if (bloodOxygenLevel < 70)
    {
        Oxygen_percentage = 70;
    }
    else if (bloodOxygenLevel < 90)
    {
        Oxygen_percentage = 60;
    }
    else if (bloodOxygenLevel < 94)
    {
        Oxygen_percentage = 40;
    }
    // update variable Oxygen Percentage According to Blood Oxygen Level
}

void controlSolenoidValve(double oxygenPercentage, int breathPerMin)
{
    double inflationTime = 30.0000 / breathPerMin;
    double tAir;    // Air Solenoid Valve Time
    double tOxygen; // Oxygen Solenoid Valve Time
    double constValue;
    double Pressure = 101325;
    double airDensity = 1.225;
    double OxygenDensity = 1.355;

    constValue = (0.79 * (1.266 * oxygenPercentage - 26.67) / (100 - oxygenPercentage)) * sqrt(Pressure * OxygenDensity / getOxygenTankPressure() / airDensity);
    if (constValue > 1)
    {
        tOxygen = inflationTime;
        tAir = tOxygen / constValue;
        openSolenoidValves(tAir, tOxygen); // values in s
    }
    else
    {
        tAir = inflationTime;
        tOxygen = tAir * constValue;
        openSolenoidValves(tAir, tOxygen); // values in s
    }
}

double getOxygenTankPressure()
{
    return 12000; // return Oxygen tank pressure in pascal
}

void startAirSupply()
{
    startStepperMotor(rBPM, rBL);
    controlSolenoidValve(0, rBPM);
}
int checkBloodOxygenLevel()
{

    return 39; // return Blood Oxygen Level
}

int PatientTemp()
{
    return 37; // return temperature value
}

void notifySpeaker()
{
    DDRD = DDRD | (1 << 4);
}

bool checkPatientTemp()
{
    if (PatientTemp() > 37.2 || PatientTemp() < 36.1)
    {
        notifyGSM("Temperature Not Normal-", PatientTemp());
        char Spercentage[4];
        itoa(PatientTemp(), Spercentage, 10); // convert int to string
        notifyDisplay(concatS("Abnormal Temp-", Spercentage));
        notifySpeaker();
        return 0;
    }
    else
    {
        return 1;
    }
    // if normal return 1 else notify speaker and gsm
}

void getParametersFromKnobs()
{

    Oxygen_percentage = rOP;
    // get values and update  Breath per min,Oxygen Percentage,Breath Length
}

bool automationOn()
{
    return OxygenAutomation; // Check Automation On/Off
}

bool turnOn()
{
    return power; // return 1 if power on switched pressed else return 0
}

bool checkStatus()
{
    if (turnOn())
    {
        if (oxygenTankPercentage() < 10)
        {
            notifyGSM("Oxygen Tank Percentage : ", oxygenTankPercentage());
            char Spercentage[4];
            itoa(oxygenTankPercentage(), Spercentage, 10); // convert int to string
            notifyDisplay(concatS("OxyTank % ", Spercentage));
        }
        if (!checkPower())
        {
            if (powerfailure == NULL)
            {
                lcd_msg("POWER FAILURE");
                lcd_cmd(0x01);
                powerfailure = 1;
            }
            notifySpeaker();
        }
        else
        {
            powerfailure = NULL;
            DDRD = DDRD | (0 << 4);
        }
        return 1;
    }
    else
    {
        return 0;
    }
}

void notifyDisplay(const char *string)
{
    lcd_msg(string);
    lcd_cmd(0x01);
}

bool checkPower()
{
    if ((PINA & (1 << PINA7)) == (1 << PINA7))
    {
        return 1; // pin is high
    }
    else
    {
        return 0; // pin is low
    }
}

void notifyGSM(const char *string, int percentage)
{
    char Spercentage[4];
    itoa(percentage, Spercentage, 10); // convert int to string

    sendSMS(Mobile_no, concatS(string, Spercentage));
}
const char *concatS(const char *string, char sPercentage[4])
{
    char *result = malloc(strlen(string) + strlen(sPercentage) + 1);
    strcpy(result, string);
    strcat(result, sPercentage);
    return result;
}

int oxygenTankPercentage()
{
    return 80;
}
int checkPatientExpPresure(){
	return 2;
}