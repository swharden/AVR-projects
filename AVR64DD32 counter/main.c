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
	CCP = CCP_IOREG_gc; // Protected write
	CLKCTRL.OSCHFCTRLA = CLKCTRL_FRQSEL_24M_gc; // Set clock to 24MHz
	
	setup_serial();
	setup_led();
	
	TCD0.CTRLA = TCD_CLKSEL_CLKPER_gc;
	//TCD0.CTRLA = TCD_CLKSEL_EXTCLK_gc;
	TCD0.INTCTRL = TCD_OVF_bm; // Enable overflow interrupt
	TCD0.CMPBCLR = 0x0FFF; // Set the top
	
	// wait for sync before enabling
	while(!(TCD0.STATUS & TCD_ENRDY_bm)){}
	TCD0.CTRLA |= TCD_ENABLE_bm;
	
	sei();
	
	uint32_t count_last = 0;
	
	while (1)
	{
		led_toggle();
		_delay_ms(100);
		
		uint32_t tmp;
		TCD0.CTRLE = TCD_SCAPTUREA_bm;
		//while ((TCD0.STATUS & TCD_CMDRDY_bm) == 0);
		//tmp = TCD0.CAPTUREA;
		
		uint32_t count_now;
		count_now = OVERFLOW_COUNT << 24;
		count_now += tmp;
		
		uint32_t count_diff = count_now - count_last;
		count_last = count_now;
		
		printf("%lu\r\n", OVERFLOW_COUNT);
	}
}
