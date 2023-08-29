#define F_CPU 24000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/cpufunc.h>

#include "Serial.h"

volatile uint32_t OVERFLOW_COUNT;

ISR(TCD0_OVF_vect)
{
	TCD0.INTFLAGS = TCD_OVF_bm;
	OVERFLOW_COUNT++;
}

void setup_led(){
	PORTD.DIRSET = PIN7_bm;
}

void led_toggle(){
	PORTD.OUTTGL = PIN7_bm;
}

int main(void)
{
	// Use the highest frequency system clock (24 MHz)
	CCP = CCP_IOREG_gc;
	CLKCTRL.OSCHFCTRLA = CLKCTRL_FRQSEL_24M_gc;
	
	// Enable the highest frequency external clock on pin 30
	CCP = CCP_IOREG_gc;
	CLKCTRL.XOSCHFCTRLA = CLKCTRL_FRQRANGE_32M_gc| CLKCTRL_SELHF_bm | CLKCTRL_ENABLE_bm;
	
	// Setup TCD to count the external clock
	TCD0.CMPBCLR = 0x0FFF; // count to max
	TCD0.CTRLA = TCD_CLKSEL_EXTCLK_gc;
	TCD0.INTCTRL = TCD_OVF_bm; // Enable overflow interrupt
	while (!(TCD0.STATUS & 0x01)); // ENRDY
	TCD0.CTRLA |= TCD_ENABLE_bm; // EXTCLK, enable
	
	// Enable global interrupts
	sei();
	
	// Setup the other peripherals
	setup_serial();
	setup_led();
	
	uint32_t count_last = 0;
	
	while (1)
	{
		led_toggle();
		_delay_ms(100);
		
		uint32_t tmp = 0;
		TCD0.CTRLE = TCD_SCAPTUREA_bm;
		while ((TCD0.STATUS & TCD_CMDRDY_bm) == 0);
		tmp = TCD0.CAPTUREA;
		
		uint32_t count_now;
		count_now = OVERFLOW_COUNT * 0x0FFF;
		count_now += tmp;
		
		uint32_t count_diff = count_now - count_last;
		count_last = count_now;
		
		unsigned long count_diff2 = count_diff*10;
		
		printf("%lu\r\n", count_diff2);
	}
}
