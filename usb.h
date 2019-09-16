#include <avr/io.h>
#include <string.h>

#ifndef USB_H_
#define USB_H_

#define USART_BAUDRATE 9600
#define BAUD_PRESCALE (((F_CPU/(USART_BAUDRATE*16UL)))-1)

void usb_init(void);

void read(char *message);

#endif
