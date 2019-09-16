
#include "usb.h"

void usb_init()
{
	UCSR0B |= (1<<RXEN0) | (1<<TXEN0);
	UCSR0C = (1<<USBS0) | (3<<UCSZ00);	UBRR0  = BAUD_PRESCALE;
}

void read(char *message)
{
	int len = strlen(message);
	
	for(int i = 0; i < len; i++)
	{
		while(!(UCSR0A & (1 << UDRE0)));
		UDR0 = message[i];
	}
}
