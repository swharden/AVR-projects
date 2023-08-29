/*

AVR64DD32 Frequency Counter

Counter:
The signal to be counted is passed into EXTCLK (pin 30)
and is counted asynchronously using PD0.

Time base:
A 10 MHz time base is clocked into XTAL32K1 (pin 20) and
divided down so the count is measured exactly ten times per second.

*/
#define F_CPU 24000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/cpufunc.h>
#include <avr/pgmspace.h>
#include "audio.h"
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

volatile uint32_t COUNTER;
ISR(TCD0_OVF_vect)
{
	TCD0.INTFLAGS = TCD_OVF_bm;
	COUNTER+=4096;
}

volatile uint32_t AUDIO_INDEX = 0;
ISR(RTC_CNT_vect){ // called 78.125 kHz
	update_count();
	RTC.INTFLAGS = 0x11; // clear the flag
}

uint8_t COUNT_NEW = 0;
uint32_t COUNT_DISPLAY = 0;
uint32_t COUNT_NOW = 0;
uint32_t COUNT_PREVIOUS = 0;
void update_count(){
	TCD0.CTRLE = TCD_SCAPTUREA_bm;
	while ((TCD0.STATUS & TCD_CMDRDY_bm) == 0);
	COUNT_NOW = COUNTER + TCD0.CAPTUREA;
	COUNT_DISPLAY = COUNT_NOW - COUNT_PREVIOUS;
	COUNT_DISPLAY = COUNT_DISPLAY * 5; // according to gate
	COUNT_PREVIOUS = COUNT_NOW;
	COUNT_NEW = 1;
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
	TCD0.CTRLA = TCD_CLKSEL_EXTCLK_gc; // count external clock input
	TCD0.INTCTRL = TCD_OVF_bm; // Enable overflow interrupt
	while (!(TCD0.STATUS & 0x01)); // ENRDY
	TCD0.CTRLA |= TCD_ENABLE_bm; // EXTCLK, enable
}

void setup_rtc_gate(){
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

void setup_TCB_PWM(){
	// Enable this peripheral
	TCB0.CTRLA |= TCB_ENABLE_bm;
	
	// Make waveform output available on the pin
	TCB0.CTRLB |= TCB_CCMPEN_bm;
	
	// Enable 8-bit PWM mode
	TCB0.CTRLB |= TCB_CNTMODE_PWM8_gc;
	
	// Set period and duty
	TCB0.CCMPL = 255; // top value
	TCB0.CCMPH = 50; // flip value
}

int main(void)
{
	setup_system_clock_24MHz();
	setup_serial();
	setup_led();
	setup_extclk_counter();
	setup_rtc_gate();
	setup_TCB_PWM();
	
	sei(); // Enable global interrupts
	
	printf("\r\nSTARTING...\r\n");
	while (1){
		if(COUNT_NEW){
			COUNT_NEW = 0;
			print_with_commas(COUNT_DISPLAY);
		}
	}
}
