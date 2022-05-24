/*
 * LM35_Temperature_Sensor.h
 *
 * Created: 5/24/2022 8:41:46 PM
 *  Author: Kalana
 */ 


#ifndef LM35 TEMPERATURE SENSOR_H_
#define LM35 TEMPERATURE SENSOR_H_

int ADC_Read_Temperature(char channel)
{
	ADMUX = 0x40 | (channel & 0x07);   /* set input channel to read */
	ADCSRA |= (1<<ADSC);               /* Start ADC conversion */
	while (!(ADCSRA & (1<<ADIF)));     /* Wait until end of conversion by polling ADC interrupt flag */
	ADCSRA |= (1<<ADIF);               /* Clear interrupt flag */
	_delay_ms(1);                      /* Wait a little bit */
	return ADCW;                       /* Return ADC word */
}

float get_Temperature()
{
	float celsius;
	celsius = (ADC_Read_Temperature(5)*4.88);
	celsius = (celsius/10.00);
	return celsius;
}



#endif /* LM35 TEMPERATURE SENSOR_H_ */