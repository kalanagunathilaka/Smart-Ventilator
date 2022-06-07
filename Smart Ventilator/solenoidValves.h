/*
 * solenoidValves.h
 *
 * Created: 5/22/2022 5:46:50 PM
 *  Author: Yesitha Sathsara
 */ 


#ifndef SOLENOIDVALVES_H_
#define SOLENOIDVALVES_H_

unsigned long case_num1;
unsigned long prev_millis1;
unsigned long need_millis1;
void openSolenoidValves(double oxygen, double air) {
	
	air=air*1000;
	oxygen=oxygen*1000;
	
	if(air>oxygen){
		if(prev_millis1==NULL){ // check whether this is the first time program reach to this line 
			
			prev_millis1=millis(); // get how many ms from the beginning of program 
			need_millis1=oxygen+prev_millis1; // at what time(ms) next action need to be done 
			case_num1=1;
			PORTC = PORTC | (1<<2);  //open oxygen(normally closed valve)
			PORTC = PORTC & (~(1<<3)); //open air (normally open valve)
			case_num1++;
			}else if(need_millis1<millis()){ // check whether next action time reached  
			
			switch(case_num1){
				
				case 2:{PORTC = PORTC & (~(1<<2)); //close oxygen
						case_num1++;
						need_millis1=need_millis1+air-oxygen; // at what time(ms) next action need to be done 
						break;
						} 
				case 3:{PORTC = PORTC | (1<<3); //close air
						case_num1++;
						need_millis1=need_millis1+air; // at what time(ms) next action need to be done 
						break;
						}
				default:{case_num1=NULL; // to repeat process set values to null
						prev_millis1=NULL;
						}
			}
			
			
			
			}else {
			if(prev_millis1==NULL){ // check whether this is the first time program reach to this line 
				
				prev_millis1=millis(); // get how many ms from the beginning of program 
				need_millis1=air+prev_millis1; // at what time(ms) next action need to be done 
				case_num1=1;
				PORTC = PORTC | (1 << 2); //open oxygen(normally closed valve)
				PORTC = PORTC & (~(1 << 3)); //open air (normally open valve)
				case_num1++;
				}else if(need_millis1<millis()){ // check whether next action time reached  
				
				switch(case_num1){
					
					case 2:{PORTC = PORTC | (1<<3); //close air
							case_num1++;
							need_millis1=need_millis1+oxygen-air; // at what time(ms) next action need to be done 
							break;
							} 
					case 3:{PORTC = PORTC & (~(1<<2)); //close oxygen
							case_num1++;
							need_millis1=need_millis1+oxygen; // at what time(ms) next action need to be done 
							break;
							}
					default:{case_num1=NULL; // to repeat process set values to null
							prev_millis1=NULL;
						}
				}
				
			}

		}

	}
}






#endif /* SOLENOIDVALVES_H_ */