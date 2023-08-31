/*

AVR64DD32 Frequency Counter

Counter:
The signal to be counted is passed into EXTCLK (pin 30)
and is counted asynchronously using PD0.

Time base:
A 10 MHz time base is clocked into XTAL32K1 (pin 20) and
divided down so the count is measured exactly once per second.

*/
#define F_CPU 24000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/cpufunc.h>

#include "Serial.h"

void led_toggle(){
	PORTD.OUTTGL = PIN7_bm;
}

volatile uint32_t COUNTER;
ISR(TCD0_OVF_vect)
{
	TCD0.INTFLAGS = TCD_OVF_bm;
	COUNTER+=4096;
}

uint8_t RTC_OVERFLOWS = 0;
uint8_t COUNT_NEW = 0;
uint32_t COUNT_DISPLAY = 0;
uint32_t COUNT_NOW = 0;
uint32_t COUNT_PREVIOUS = 0;

void handle_5hz_tick(){
	RTC_OVERFLOWS++;
	if (RTC_OVERFLOWS == 5){
		RTC_OVERFLOWS = 0;
		TCD0.CTRLE = TCD_SCAPTUREA_bm;
		while ((TCD0.STATUS & TCD_CMDRDY_bm) == 0);
		COUNT_NOW = COUNTER + TCD0.CAPTUREA;
		COUNT_DISPLAY = COUNT_NOW - COUNT_PREVIOUS;
		COUNT_PREVIOUS = COUNT_NOW;
		COUNT_NEW = 1;
	}
}

ISR(RTC_CNT_vect){
	handle_5hz_tick();
	RTC.INTFLAGS = 0x11;
}

ISR(TCA0_OVF_vect){
	handle_5hz_tick();
	TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;
}

void setup_led(){
	PORTD.DIRSET = PIN7_bm;
}

void setup_system_clock_24MHz(){
	// Enable clock output on pin 5
	CCP = CCP_IOREG_gc;
	CLKCTRL.MCLKCTRLA = CLKCTRL_CLKOUT_bm;
	
	// Use the highest frequency system clock (24 MHz)
	CCP = CCP_IOREG_gc;
	CLKCTRL.OSCHFCTRLA = CLKCTRL_FRQSEL_24M_gc | CLKCTRL_CLKOUT_bm;
}

void setup_extclk_counter(){
	// Enable the highest frequency external clock on pin 30
	CCP = CCP_IOREG_gc;
	CLKCTRL.XOSCHFCTRLA = CLKCTRL_FRQRANGE_32M_gc| CLKCTRL_SELHF_bm | CLKCTRL_ENABLE_bm;
	
	// Setup TCD to count the external clock
	TCD0.CMPBCLR = 0x0FFF; // count to max
	TCD0.CTRLA = TCD_CLKSEL_EXTCLK_gc; // count external clock input
	TCD0.INTCTRL = TCD_OVF_bm; // Enable overflow interrupt
	while (!(TCD0.STATUS & 0x01)); // ENRDY
	TCD0.CTRLA |= TCD_ENABLE_bm; // EXTCLK, enable
}

void setup_gate_rtc(){
	// Enable the RTC
	CCP = CCP_IOREG_gc;
	CLKCTRL.XOSC32KCTRLA = CLKCTRL_SEL_bm | CLKCTRL_ENABLE_bm; // External clock on the XTAL32K1 pin, enable
	
	// Setup the RTC at 10 MHz to interrupt periodically
	// 10 MHz with 128 prescaler is 78,125 ticks/sec
	RTC.CTRLA = RTC_PRESCALER_DIV128_gc | RTC_RTCEN_bm;
	RTC.PER = 15624; // 5 overflows per second (78125/5-1)
	RTC.INTCTRL = RTC_OVF_bm;
	RTC.CLKSEL = RTC_CLKSEL_XTAL32K_gc; // clock in XOSC23K pin
}

void setup_gate_sysclk(){
	// 24 MHz clock div 256 is 93,750 ticks/second
	TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV256_gc | TCA_SINGLE_ENABLE_bm;
	
	// enable overflow interrupt
	TCA0.SINGLE.INTCTRL |= TCA_SINGLE_OVF_bm;
	
	// overflow 5 times per second
	TCA0.SINGLE.PER = 18750-1;
}

int main(void)
{
	setup_system_clock_24MHz();
	setup_serial();
	setup_led();
	setup_extclk_counter();
	
	// Use one gate source or the other but not both
	//setup_gate_rtc(); // Gate using 10 MHz signal on pin 20
	setup_gate_sysclk(); // Gate using system clock
	
	sei(); // Enable global interrupts
	
	printf("\r\nSTARTING...\r\n");
	while (1){
		if(COUNT_NEW){
			COUNT_NEW = 0;
			print_with_commas(COUNT_DISPLAY);
		}
	}
}
