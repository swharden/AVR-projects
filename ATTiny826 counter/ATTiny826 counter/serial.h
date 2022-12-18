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

static void SERIAL_sendComma(){
	USART0_sendChar(',');
}

static void SERIAL_sendBreak(){
	USART0_sendChar('\r');
	USART0_sendChar('\n');
}

static void SERIAL_sendUnsignedLong(unsigned long count){
	unsigned long int divby=1000000000;
	while (divby){
		USART0_sendChar('0'+count/divby);
		count-=(count/divby)*divby;
		divby/=10;
	}
}
