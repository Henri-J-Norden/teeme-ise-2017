/*
 * Created: 13.11.2017 18:08:51
 *  Author: Robotiklubi
 */ 

#include "adc.h"

void adc_init()
{
	ADCSRA = (0x7 << ADPS0); //set prescaler to 128
	ADCSRB = 0; // algv22rtusta register

	ADMUX = (1 << REFS0);  // set reference voltage to Vcc
	ADCSRA |= (1 << ADEN); // Turn ADC on
}

uint16_t read_adc(uint8_t channel)
{
	ADMUX &= 0xF0;     //Clear the older channel that was read
	ADMUX |= channel;  //Defines the new ADC channel to be read
	if((channel < 9) || (channel == 14) || (channel == 15))
	{
		ADMUX |= (channel << MUX0); //Vali ADC sisendiks kanal mis on m22ratud muutujaga channel
	}
	else
	{
		return 0xFFFF;
	}

	ADCSRA |= (1 << ADSC);

	while(ADCSRA & (1 << ADSC));

	return ADC;
}