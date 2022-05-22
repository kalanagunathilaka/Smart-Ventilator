/*
 * stepper_motor.h
 *
 * Created: 5/22/2022 5:55:39 PM
 *  Author: Nelaka Gunawardana
 */ 


#ifndef STEPPER MOTOR_H_
#define STEPPER MOTOR_H_



unsigned long prev_millis0;
unsigned long need_millis0;
int case_num0;

void startStepperMotor(int breathPerMin, int BreathLength) {
	if(prev_millis0==NULL){
		
		prev_millis0=millis();
		need_millis0=30000/(breathPerMin*10)+prev_millis0;
		case_num0=1;
		PORTC = PORTC | (1<<4);case_num0++;
		}else if(need_millis0<millis()){
		
		switch(case_num0){
			
			case 2:{PORTC = PORTC | (1<<6);
				case_num0++;
				need_millis0=need_millis0+30000/(breathPerMin*10);
				break;}
			case 3:{PORTC = PORTC & (~(1<<4));
				case_num0++;need_millis0=need_millis0+30000/(breathPerMin*10);
				break;}
			case 4:{PORTC = PORTC | (1<<5);
				case_num0++;need_millis0=need_millis0+30000/(breathPerMin*10);
				break;}
			case 5:{PORTC = PORTC & (~(1<<6));
				case_num0++;need_millis0=need_millis0+30000/(breathPerMin*10);
				break;}
			case 6:{PORTC = PORTC | (1<<7);case_num0++;
				need_millis0=need_millis0+30000/(breathPerMin*10);
				break;}
			case 7:{PORTC = PORTC & (~(1<<5));case_num0++;need_millis0=need_millis0+30000/(breathPerMin*10);
				break;}
			case 8:{PORTC = PORTC | (1<<4);case_num0++;
				need_millis0=need_millis0+30000/(breathPerMin*10);
				break;}
			case 9:{PORTC = PORTC & (~(1<<7));case_num0++;
				need_millis0=need_millis0+30000/(breathPerMin*10);
				break;}
			case 10:{PORTC = PORTC & (~(1<<4));
				case_num0++;need_millis0=need_millis0+30000/(breathPerMin*10);
				break;}
			case 11:{PORTC = PORTC | (1<<4);case_num0++;
				need_millis0=need_millis0+30000/(breathPerMin*10);
				break;}
			case 12:{PORTC = PORTC | (1<<7);
				case_num0++;need_millis0=need_millis0+30000/(breathPerMin*10);
				break;}
			case 13:{PORTC = PORTC & (~(1<<4));
				case_num0++;need_millis0=need_millis0+30000/(breathPerMin*10);
				break;}
			case 14:{PORTC = PORTC | (1<<5);case_num0++;
				need_millis0=need_millis0+30000/(breathPerMin*10);
				break;}
			case 16:{PORTC = PORTC & (~(1<<7));case_num0++;need_millis0=need_millis0+30000/(breathPerMin*10);
				break;}
			case 17:{PORTC = PORTC | (1<<6);case_num0++;
				need_millis0=need_millis0+30000/(breathPerMin*10);
				break;}
			case 18:{PORTC = PORTC & (~(1<<5));case_num0++;
				need_millis0=need_millis0+30000/(breathPerMin*10);break;}
			case 19:{PORTC = PORTC & (~(1<<6));case_num0++;need_millis0=need_millis0+30000/(breathPerMin*10);
				break;}
			case 20:{PORTC = PORTC & (~(1<<4));case_num0++;
				need_millis0=need_millis0+30000/(breathPerMin*10);
				break;}
			default:{case_num0=NULL;
				prev_millis0=NULL;
				}
		}
	}
	



	
}



#endif /* STEPPER MOTOR_H_ */