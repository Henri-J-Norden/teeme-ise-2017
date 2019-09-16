/*
 * Robot489.c
 *
 * Created: 13.11.2017 18:06:10
 * Author : Robotiklubi ja sõbrad
 */ 

#include <avr/io.h>
#include <string.h>
#include <util/delay.h>
#include <stdio.h>
#include <inttypes.h>

#include "adc.h"
#include "usb.h"
#include "motors.h"

// Distance sensors
#define RC 4
#define LC 3
#define RF 2
#define LF 1

/// Constant values
#define _DEFAULT_SPEED 1024
//	 Maze constants
#define DIVIDER 150 // ~ max expected difference
#define SSTOP 450
#define SLOW 375
//	Line sensor constants
#define BLACK_VALUE 725
#define SPEED_PID 400 // Kui üks äär näeb
#define SPEED_PID2 300 // üks äär ei näe
#define SPEED_PID3 800 // kui midagi ei näe


// Debug
#define SERIAL 0

/// Globals
int16_t DEFAULT_SPEED = _DEFAULT_SPEED;
//	 Speed arrays
int16_t S[2] = {_DEFAULT_SPEED, _DEFAULT_SPEED};
int16_t SO[2] = {_DEFAULT_SPEED, _DEFAULT_SPEED};
//	Debug
char text[100];

int max(int a, int b) { return a > b ? a : b; }
int min(int a, int b) { return a < b ? a : b; }

uint16_t get_range(uint8_t ch) {
	int32_t U = read_adc(ch);
	if (SERIAL) {
		sprintf(text, "RAWVAL: %li\n", U);
		read(text);
	}
	
	if (U < 100) {
		return 300;
	} else if (U < 164) { // 164
		//return (544000 - 2344 * U) / 1000;
		//return (387499 - 1388 * U) / 1000;
		return (330591 - 1041 * U) / 1000;
		//return (273683 - 694 * U) / 1000;
	} else if (U < 481) { // values starting from 159867 / 1000
		return (216775 - 347 * U) / 1000;
	} else if (U < 614) {
		return (104231 - 112 * U) / 1000;
	} else {
		return 35;
	}
}


void set_maze_speeds() {
	int16_t l = get_range(LF);
	int16_t r = get_range(RF);
	int16_t lc = read_adc(LC);
	int16_t rc = read_adc(RC);
	int32_t d = l - r;
	
	// decrease max speed if near wall
	if (lc > SLOW || rc > SLOW) {
		if (DEFAULT_SPEED == _DEFAULT_SPEED) DEFAULT_SPEED = 500;
	} else if (DEFAULT_SPEED != _DEFAULT_SPEED) {
		DEFAULT_SPEED = _DEFAULT_SPEED;
	}
	
	if (SERIAL) {
		sprintf(text, "-RNG- L: %i R: %i DIFF: %li\n", l, r, d);
		read(text);
	}
	
	// sets the speed based on the difference of values from the 2 front sensors
	if (d < 0) {
		S[0] = DEFAULT_SPEED;
		S[1] = DEFAULT_SPEED + max(d * DEFAULT_SPEED / DIVIDER, - 2 * DEFAULT_SPEED);
	} else {
		S[1] = DEFAULT_SPEED;
		S[0] = DEFAULT_SPEED - min(d * DEFAULT_SPEED / DIVIDER, 2 * DEFAULT_SPEED);
	}
	
	// averaging with the previous value
	for (int i = 0; i < 2; i++) { 		
		if (S[i] - SO[i] < 0) S[i] = SO[i] + ((S[i] - SO[i]) / 10);
		SO[i] = S[i];
	}
	
	// STOP and turn right if wall is close
	if (lc > SSTOP || rc > SSTOP) {
		S[0] = -200;
		S[1] = -1000;
	}
	
	if (SERIAL) {
		sprintf(text, "-SPD- L: %i R: %i\n\n", S[0], S[1]);
		read(text);
	}
}


void run_maze(void) {
	while (1) {
		set_maze_speeds();
		motors_set(S[0], S[1]);
	}
}


int isBlack(uint8_t channel) {
	return read_adc(channel) > BLACK_VALUE;
}


void set_line_speeds (uint8_t *JooneAsukoht, uint8_t KasNagi) {
	S[0]= DEFAULT_SPEED;
	S[1]= DEFAULT_SPEED;	

	if ((JooneAsukoht[0]==1)&& (JooneAsukoht[1] == 0)&&(JooneAsukoht[2] == 0) && KasNagi) { // pööra kõvasti vasakule
		S[0] -= SPEED_PID2;
		S[1] = DEFAULT_SPEED;
	} else if ((JooneAsukoht[0] == 0)&&(JooneAsukoht[1] == 0)&&(JooneAsukoht[2] == 1) && KasNagi) { // pööra kõvasti paremale
		S[0] = DEFAULT_SPEED;
		S[1] -= SPEED_PID2;
	} else if ((JooneAsukoht[0] == 1)&&(JooneAsukoht[1] == 0)&&(JooneAsukoht[2] == 0) && !KasNagi) { // pööra kõvasti kõvasti vasakule
		S[0] = -100;
		S[1] -= 150;
	} else if ((JooneAsukoht[0] == 0) &&(JooneAsukoht[1] == 0)&&(JooneAsukoht[2] == 1) && !KasNagi) { // pööra kõvasti kõvasti paremale
		S[0] -= 150;
		S[1] = -100;
	} else if((JooneAsukoht[0] == 1) && (JooneAsukoht[1] == 1) && (JooneAsukoht[2] == 1)) { //otse
		S[0]= DEFAULT_SPEED;
		S[1]= DEFAULT_SPEED;
	} else if((JooneAsukoht[0] == 1) && (JooneAsukoht[1] == 1) && (JooneAsukoht[2] == 0)) { // natuke veidi vasakule
		S[1] = DEFAULT_SPEED;
		S[0] -= SPEED_PID2;
	} else if((JooneAsukoht[0] == 0) && (JooneAsukoht[1] == 1) && (JooneAsukoht[2] == 1)) { // natuke veidi paremale
		S[0] = DEFAULT_SPEED;
		S[1] -= SPEED_PID2;
	}
}


#define OBSTACLE_DIST 400

void avoid_obstacle(void) {
	motors_set(-1000, -1000); // GET BACK
	_delay_ms(100);
	motors_set(-1000, 0); // turn to the left
	_delay_ms(280);
	motors_set(1000, 600); // roll around
	_delay_ms(1500);
	while (1) { // keep going, hell yeah
		if (isBlack(5) && isBlack(6) && isBlack(7)) break; // while not all black, keep looking for a better match ;)
	}
	motors_set(-200, 500); // turn it back baby
	while (1) {
		if (isBlack(5)) {
			motors_set(0, 0);
			break;
		}
	}
}

void run_line(void) {
	uint8_t i;
	uint8_t lineLocation[3] = {0,0,0};
	uint8_t oldLineLocation[3] = {0,0,0};
	uint8_t lineSeen;
	
	while (1) {
		lineSeen = 0;
		
		// check for obstacles
		if (read_adc(LC) > OBSTACLE_DIST || read_adc(RC) > OBSTACLE_DIST) { 
			avoid_obstacle();
		}
		
		for(i= 0; i < 3; i++) {
			lineLocation[i] = isBlack(i+5); // alates ch5 on jooneandurid
			lineSeen += lineLocation[i];
		}
		
		if (!lineSeen) {
			set_line_speeds(oldLineLocation, lineSeen);
			motors_set(S[0], S[1]);
		} else {
			set_line_speeds(lineLocation, lineSeen);
			for ( i = 0; i < 3; i++) {
				oldLineLocation[i] = lineLocation[i];
				motors_set(S[0], S[1]);
			}
		}
		
		lineSeen = 0;
		for(i= 0; i < 3; i++) {
			lineLocation[i] = isBlack(i+5); // alates ch5 on jooneandurid
			lineSeen += lineLocation[i];
		}
		
		if (!lineSeen) {
			set_line_speeds(oldLineLocation, lineSeen);
			motors_set(S[0], S[1]);
			} else {
			set_line_speeds(lineLocation, lineSeen);
			for ( i = 0; i < 3; i++) {
				oldLineLocation[i] = lineLocation[i];
				motors_set(S[0], S[1]);
			}
		}
	}
}


int main(void) {	
	adc_init();
	usb_init();
	motors_init();
	
	if (read_adc(0) >= 512) {
		run_maze();
	} else {
		run_line();
	}
}
