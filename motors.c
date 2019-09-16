
#include "motors.h"

void motors_init(void)
{
	DDRB|=(1<<DDB0)|(1<<DDB1)|(1<<DDB2)|(1<<DDB3);
	
	TCCR1A|=(1<<COM1A1|1<<COM1B1);
	
	TCCR1A|=1<<WGM11;
	TCCR1B|=(1<<WGM12) | (1<<WGM13);
	
	TCCR1B|=(1<<CS10);
	
	ICR1 = 1000;
}

void motors_set(int16_t left, int16_t right)
{
	left = left * (-1);
	if(right>=0)
	{
		PORTB|=(1<<BPH);
		OCR1B = right;
	}
	else
	{
		PORTB &=~(1<<BPH);
		OCR1B = (-1)*right;
	}
	
	if(left>=0)
	{
		PORTB|=(1<<APH);
		OCR1A = left;
	}
	else
	{
		PORTB &=~(1<<APH);
		OCR1A = (-1)*left;
	}
}
