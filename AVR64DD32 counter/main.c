/*

AVR64DD32 Frequency Counter

Counter:
The signal to be counted is passed into EXTCLK (pin 30)
and is counted asynchronously using PD0.

Time base:
A 28.704 MHz crystal is clocked into XTAL32K1 (pin 20) and
divided down to produce an interrupt 5 times per second.

*/
#define F_CPU 24000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/cpufunc.h>

#include "Serial.h"

void print_with_commas(unsigned long freq){
	int millions = freq / 1000000;
	freq -= millions * 1000000;
	int thousands = freq / 1000;
	freq -= thousands * 1000;
	int ones = freq;
	printf("%d,%03d,%03d\r\n", millions, thousands, ones);
}

void led_toggle(){
	PORTD.OUTTGL = PIN7_bm;
}

volatile uint32_t COUNTER_OVERFLOWS;
ISR(TCD0_OVF_vect)
{
	TCD0.INTFLAGS = TCD_OVF_bm;
	COUNTER_OVERFLOWS++;
}

volatile uint8_t TIMER_READY = 0;
volatile int TIMER_OVERFLOWS = 0;
uint32_t count_last = 0;
ISR(RTC_CNT_vect){
	RTC.INTFLAGS = RTC_OVF_bm;
	TIMER_OVERFLOWS++;
	led_toggle();
	if (TIMER_OVERFLOWS < 5)
	return;
	
	TIMER_OVERFLOWS = 0;

	uint32_t tmp = 0;
	TCD0.CTRLE = TCD_SCAPTUREA_bm;
	while ((TCD0.STATUS & TCD_CMDRDY_bm) == 0);
	tmp = TCD0.CAPTUREA;

	uint32_t count_now;
	count_now = COUNTER_OVERFLOWS * 0x0FFF;
	count_now += tmp;

	uint32_t count_diff = count_now - count_last;
	count_last = count_now;

	print_with_commas(count_diff);
}

void setup_led(){
	PORTD.DIRSET = PIN7_bm;
}

void setup_system_clock_24MHz(){
	// Use the highest frequency system clock (24 MHz)
	CCP = CCP_IOREG_gc;
	CLKCTRL.OSCHFCTRLA = CLKCTRL_FRQSEL_24M_gc;
}

void setup_extclk_counter(){
	// Enable the highest frequency external clock on pin 30
	CCP = CCP_IOREG_gc;
	CLKCTRL.XOSCHFCTRLA = CLKCTRL_FRQRANGE_32M_gc| CLKCTRL_SELHF_bm | CLKCTRL_ENABLE_bm;
	
	// Setup TCD to count the external clock
	TCD0.CMPBCLR = 0x0FFF; // count to max
	TCD0.CTRLA = TCD_CLKSEL_EXTCLK_gc;
	TCD0.INTCTRL = TCD_OVF_bm; // Enable overflow interrupt
	while (!(TCD0.STATUS & 0x01)); // ENRDY
	TCD0.CTRLA |= TCD_ENABLE_bm; // EXTCLK, enable
}

void setup_rtc_gate(){
	// Enable the RTC
	CCP = CCP_IOREG_gc;
	CLKCTRL.XOSC32KCTRLA = CLKCTRL_SEL_bm | CLKCTRL_ENABLE_bm; // External clock on the XTAL32K1 pin, enable
	
	// Setup the RTC at 10 MHz to interrupt periodically
	// 28.704 MHz with 256 prescaler is 112,125 ticks/sec
	RTC.CTRLA = RTC_PRESCALER_DIV256_gc | RTC_RTCEN_bm;
	RTC.INTCTRL = RTC_OVF_bm; // interrupt on overflow
	RTC.CLKSEL = RTC_CLKSEL_XTAL32K_gc; // clock in XOSC23K pin
	RTC.PER = 22425; // set period for 5 overflows per second
	
}

int main(void)
{
	setup_system_clock_24MHz();
	setup_serial();
	setup_led();
	setup_extclk_counter();
	setup_rtc_gate();
	
	sei(); // Enable global interrupts
	
	printf("\r\nSTARTING...\r\n");
	while (1){}
}
