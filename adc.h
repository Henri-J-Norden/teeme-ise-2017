/*
 * adc.h
 *
 * Created: 13.11.2017 18:09:40
 *  Author: Robotiklubi
 */ 


#ifndef ADC_H_
#define ADC_H_
#include <avr/io.h>

void adc_init();

uint16_t read_adc(uint8_t channel);

#endif 
