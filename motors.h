
#ifndef MOTORS_H_
#define MOTORS_H_


#include <avr/io.h>
#define APH PB0
#define AEN PB1 //OC1A!
#define BEN PB2 //OC1B!
#define BPH PB3

void motors_init(void);

void motors_set(int16_t left, int16_t right);

#endif
