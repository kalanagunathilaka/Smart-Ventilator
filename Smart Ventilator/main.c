/*
 * Smart Ventilator.c
 *
 * Created: 11/27/2021 1:26:16 PM
 * Author : Yesitha Sathsara
 */ 

#include <avr/io.h>
#include <stdbool.h>


bool checkStatus();

int oxygenTankPercentage();

void notifyGSM(const char *string, int percentage);

void notifyDisplay();

bool automationOn();

void getParametersFromKnobs();

bool checkPatientTemp();

int PatientTemp();

int checkBloodOxygenLevel();



void startAirSupply();

void notifySpeaker();

bool turnOn();

void startStepperMotor(int breathPerMin, int BreathLength);

void startOxygenAndAirSupply(int percentage);

void controlSolenoidValve(int oxygenPercentage, int breathPerMin);

int calculateConstant(int pressure, int density);

int getOxygenTankPressure();

int calculateAnotherTime(int value);

int Average_Blood_Oxygen_level=97;
int Average_Breath_length=50;
int Average_Breath_Per_Min=18;
int Oxygen_percentage=60;

int main(void)
{
	/* Replace with your application code */

	while (1)
	{

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

	void startOxygenAndAirSupply(int percentage) {
		startStepperMotor(Average_Breath_Per_Min, Average_Breath_length);
		controlSolenoidValve(Oxygen_percentage, Average_Breath_Per_Min);
	}

	void controlSolenoidValve(int oxygenPercentage, int breathPerMin) {
		int inflationTime=30/breathPerMin;
		int tAir;//Air Solenoid Valve Time
		int tOxygen;//Oxygen Solenoid Valve Time
		int constValue;
		int Pressure;
		int airDensity;
		int OxygenDensity;
		if(tAir>tOxygen){
			tAir=inflationTime;
			constValue=calculateConstant(Pressure, airDensity);
			tOxygen=calculateAnotherTime(constValue);
			} else{
			tOxygen=inflationTime;
			
			constValue=calculateConstant(getOxygenTankPressure(), OxygenDensity);
			tAir= calculateAnotherTime(constValue);
		}
	}

	int calculateAnotherTime(int value) {
		return 0;
	}

	int getOxygenTankPressure() {
		return 0;//return Oxygen tank pressure
	}

	int calculateConstant(int pressure, int density) {
		return 1;
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
			notifyGSM("Temperature Not Normal",PatientTemp());
			notifyDisplay();
			notifySpeaker();
			return 0;
			}else{
			return 1;
		}
		//if normal return 1 else notify speaker and gsm
	}

	void getParametersFromKnobs() {

		//get values Breath per min,Oxygen Percentage,Breath Length
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

			}

			int oxygenTankPercentage() {
				//return oxygen tank percentage
				return 1;
			}


