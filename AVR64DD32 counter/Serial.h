#ifndef SERIAL_H_
#define SERIAL_H_

#include <string.h>
#include <stdio.h>

#define USART0_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)

static void USART0_sendChar(char c)
{
	while (!(USART0.STATUS & USART_DREIF_bm)) {;}
	USART0.TXDATAL = c;
}

static int USART0_printChar(char c, FILE *stream)
{
	USART0_sendChar(c);
	return 0;
}

void USART0_sendString(char *str)
{
	for(size_t i = 0; i < strlen(str); i++)
	{
		USART0_sendChar(str[i]);
	}
}

static FILE USART_stream = FDEV_SETUP_STREAM(USART0_printChar, NULL, _FDEV_SETUP_WRITE);

void setup_serial(void)
{
	// use alternate pins (TX/RX on PA4/PA5)
	PORTMUX_USARTROUTEA = PORTMUX_USART0_ALT1_gc;
	
	// set TX pin as an output: PA4 (pin2)
	PORTA.DIRSET = PIN4_bm;

	// set baud rate based on CPU clock
	USART0.BAUD = (uint16_t)USART0_BAUD_RATE(9600);
	
	// enable USART TX
	USART0.CTRLB |= USART_TXEN_bm;
	
	// direct printf() output to this serial port
	stdout = &USART_stream;
}

void print_with_commas(unsigned long freq){
	int millions = freq / 1000000;
	freq -= millions * 1000000;
	int thousands = freq / 1000;
	freq -= thousands * 1000;
	int ones = freq;
	printf("%d,%03d,%03d\r\n", millions, thousands, ones);
}

#endif /* SERIAL_H_ */