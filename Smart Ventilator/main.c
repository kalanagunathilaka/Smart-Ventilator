/*
 * Smart Ventilator.c
 *
 * Created: 11/27/2021 1:26:16 PM
 * Author : Yesitha Sathsara
 */ 

#include <avr/io.h>
#include <vss.h>


bool checkStatus();

int oxygenTankPercentage();

void notifyGSM(const char *string, int percentage);

void notifySecondaryDisplay();

bool automationOn();

void getParametersFromKnobs();

bool checkPatientTemp();

int PatientTemp();

int checkBloodOxygenLevel();

void startOxygenAndAirSupply();

void startAirSupply();

int Average_Blood_Oxygen_level=97;

int main(void)
{
	/* Replace with your application code */

	while (1)
	{

		if(checkStatus()){
			if(automationOn()){
				if(checkPatientTemp()){

					if(checkBloodOxygenLevel() < Average_Blood_Oxygen_level){
						startOxygenAndAirSupply();
						}else{
						startAirSupply();
					}
				}
				}else{
				getParametersFromKnobs();
			}
			}else{return 0;}
		}
	}

	void startAirSupply() {

	}

	void startOxygenAndAirSupply() {

	}

	int checkBloodOxygenLevel() {
		return 0;
	}

	int PatientTemp() {
		return 0;//return temperature value
	}

	bool checkPatientTemp() {
		//if normal return 1 else notify speaker and gsm
	}

	void getParametersFromKnobs() {
		//get values and set to variables
	}

	bool automationOn() {
		return 1;
	}

	bool checkStatus() {
		if(oxygenTankPercentage() < 10){
			notifyGSM("Oxygen Tank Percentage : ",oxygenTankPercentage());
			notifySecondaryDisplay();
			return 0;
			}else{return 1;}
		}

		void notifySecondaryDisplay() {

		}

		void notifyGSM(const char *string, int percentage) {

		}

		int oxygenTankPercentage() {
			oxygenTankPercentage
		}



