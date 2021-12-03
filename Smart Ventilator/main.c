/*
 * Smart Ventilator.c
 *
 * Created: 11/27/2021 1:26:16 PM
 * Author : Yesitha Sathsara
 */ 

#include <avr/io.h>
#include <stdbool.h>
#include <math.h>


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

	void openSolenoidValves(double air, double oxygen) {

	}

	double calculateAnotherTime(double value, double pressure, double density, double percentage) {
		return percentage*value* sqrt(density/(2*pressure));
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
			notifyGSM("Temperature Not Normal",PatientTemp());
			notifyDisplay();
			notifySpeaker();
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

			}

			int oxygenTankPercentage() {
				//return oxygen tank percentage
			}


