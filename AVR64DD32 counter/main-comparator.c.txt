#define F_CPU 4000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include <avr/interrupt.h>

#pragma region serial

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

void setup_serial(void)
{
	PORTMUX_USARTROUTEA = PORTMUX_USART0_ALT1_gc; // alternate pins (TX/RX on PA4/PA5)
	PORTA.DIR |= PIN4_bm; // PA4 TX must be an output

	USART0.BAUD = (uint16_t)USART0_BAUD_RATE(9600);
	USART0.CTRLB |= USART_TXEN_bm;
	
	stdout = &USART_stream;
	
	printf("\r\nRESTARTED\r\n");
}

#pragma endregion serial

#pragma region LED

void setup_LED_status(){
	PORTD.DIR |= PIN7_bm;
}

void LED_status_on(){
	PORTD.OUTSET = PIN7_bm;
}

void LED_status_toggle(){
	PORTD.OUTTGL = PIN7_bm;
}

void LED_status_off(){
	PORTD.OUTCLR = PIN7_bm;
}

#pragma endregion LED

void setup_comparator(){
	
	// setup input pins: PD2 (pin12) and PD3 (pin13)
	PORTD.IN |= PIN2_bm | PIN3_bm;
	
	// setup output pin (PA7/pin5)
	PORTA.DIR |= PIN7_bm;
	
	// enable AC0 and output pin
	AC0.CTRLA |= AC_OUTEN_bm | AC_ENABLE_bm;
	
	// enable AC interrupt
	//AC0.INTCTRL = AC_CMP_bm;

	// wire AC0 event (generator) to TCD0 INPUTA (user)
	EVSYS.CHANNEL0 = EVSYS_CHANNEL0_AC0_OUT_gc;
	EVSYS.USERTCD0INPUTA = EVSYS_USER_CHANNEL0_gc;

	// enable counter and set prescaler
	TCD0.EVCTRLA = TCD_CFG_ASYNC_gc | TCD_TRIGEI_bm;
	
	// set count to maximum value (2^12)
	TCD0.CMPBCLR = 0x0FFF;
	
	// to start the counter disable it, wait, then enable it.
	TCD0.CTRLA = 0;
	while (!(TCD0.STATUS & TCD_ENRDY_bm));
	TCD0.CTRLA = TCD_ENABLE_bm; // no clock source (only event counting)
	
	// enable overflow interrupt
	TCD0.INTCTRL = TCD_OVF_bm;
}

unsigned long count = 0;
unsigned long countLast = 0;

ISR(TCD0_OVF_vect) {
	count += TCD0.CMPBCLR;
}

int main(void)
{
	setup_serial();
	setup_LED_status();
	setup_comparator();
	sei();
	
	while (1)
	{
		unsigned long total = count;
		total += TCD0.CAPTUREA;
		unsigned long diff = total - countLast;
		countLast = total;
		printf("%lu\r\n", diff);
		_delay_ms(11);
	}
}