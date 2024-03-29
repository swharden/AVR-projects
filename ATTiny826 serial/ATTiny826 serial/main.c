#define F_CPU 3333333UL
#define USART0_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>

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

static FILE USART_stream = FDEV_SETUP_STREAM(USART0_printChar, NULL, _FDEV_SETUP_WRITE);

void USART0_init(void)
{
	PORTMUX_USARTROUTEA = PORTMUX_USART0_ALT1_gc; // alternate pins (TX/RX on PA1/PA2)
	PORTA.DIR |= PIN1_bm;

	USART0.BAUD = (uint16_t)USART0_BAUD_RATE(9600);
	USART0.CTRLB |= USART_TXEN_bm;
	
	stdout = &USART_stream;
}

void USART0_sendString(char *str)
{
	for(size_t i = 0; i < strlen(str); i++)
	{
		USART0_sendChar(str[i]);
	}
}

uint8_t count = 0;
int main(void)
{
	USART0_init();
	
	PORTB.DIR = 255;
	while (1)
	{
		PORTB.OUT = 255;
		_delay_ms(100);
		PORTB.OUT = 0;
		_delay_ms(100);
		printf("Counter: %d\r\n", count++);
	}
}

