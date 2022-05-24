/*
 * MPX4250_Pressure_Sensor.h
 *
 * Created: 5/24/2022 7:06:49 PM
 *  Author: Kalana
 */ 


#ifndef MPX4250 PRESSURE SENSOR_H_
#define MPX4250 PRESSURE SENSOR_H_

int ADC_Read_Pressure(char channel)
{
	ADMUX = 0x40 | (channel & 0x07);   /* set input channel to read */
	ADCSRA |= (1<<ADSC);               /* Start ADC conversion */
	while (!(ADCSRA & (1<<ADIF)));     /* Wait until end of conversion by polling ADC interrupt flag */
	ADCSRA |= (1<<ADIF);               /* Clear interrupt flag */
	_delay_ms(1);                      /* Wait a little bit */
	return ADCW;                       /* Return ADC word */
}

float get_pressure(int pin)
{
	float kilopascal;
	kilopascal = (ADC_Read_Pressure(pin));
	kilopascal = ((((kilopascal/1024)-0.04)/0.00369)-3.3);
	return kilopascal;
}

#endif /* MPX4250 PRESSURE SENSOR_H_ */