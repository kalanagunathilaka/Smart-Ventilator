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
		if(prev_millis1==NULL){
			
			prev_millis1=millis();
			need_millis1=oxygen+prev_millis1;
			case_num1=1;
			PORTC = PORTC | (1<<2);  //open oxygen(normally closed valve)
			PORTC = PORTC & (~(1<<3)); //open air (normally open valve)
			case_num1++;
			}else if(need_millis1<millis()){
			
			switch(case_num1){
				
				case 2:{PORTC = PORTC & (~(1<<2));
						case_num1++;
						need_millis1=need_millis1+air-oxygen;
						break;
						} //close oxygen
				case 3:{PORTC = PORTC | (1<<3);
						case_num1++;
						need_millis1=need_millis1+air;
						break;
						}//close air
				default:{case_num1=NULL;
						prev_millis1=NULL;
						}
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
					
					case 2:{PORTC = PORTC | (1<<3);
							case_num1++;
							need_millis1=need_millis1+oxygen-air;
							break;
							} //close air
					case 3:{PORTC = PORTC & (~(1<<2));
							case_num1++;
							need_millis1=need_millis1+oxygen;
							break;
							}//close oxygen
					default:{case_num1=NULL;
							prev_millis1=NULL;
						}
				}
				
			}

		}

	}
}






#endif /* SOLENOIDVALVES_H_ */