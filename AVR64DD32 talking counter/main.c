/*

AVR64DD32 Frequency Counter

Counter:
The signal to be counted is passed into EXTCLK (pin 30)
and is counted asynchronously using PD0.

Time base:
A 10 MHz time base is passed into XTAL32K1 (pin 20) and
divided down so the counter is gated ten times per second.

PWM audio:
Audio waveforms are generated using PB0 outputting PWM on PA2 (pin 32).

*/
#define F_CPU 24000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/cpufunc.h>
#include <avr/pgmspace.h>
#include "NumberSpeaker.h"
#include "Serial.h"

void led_toggle(){
	PORTD.OUTTGL = PIN7_bm;
}

void led_on(){
	PORTD.OUTSET = PIN7_bm;
}

void led_off(){
	PORTD.OUTCLR = PIN7_bm;
}

volatile uint32_t COUNTER;
ISR(TCD0_OVF_vect)
{
	TCD0.INTFLAGS = TCD_OVF_bm;
	COUNTER+=4096;
}

volatile uint16_t RTC_COUNT=0;
uint8_t COUNT_NEW = 0;
uint32_t COUNT_DISPLAY = 0;
uint32_t COUNT_NOW = 0;
uint32_t COUNT_PREVIOUS = 0;
ISR(RTC_CNT_vect){
	
	// set PWM level for analog output
	if (IsPlaying()){
		TCB0.CCMPH = GetNextAudioLevel();
	}
	
	// count ticks to gate counter
	RTC_COUNT++;
	if (RTC_COUNT == 800){
		RTC_COUNT = 0;
		TCD0.CTRLE = TCD_SCAPTUREA_bm;
		while ((TCD0.STATUS & TCD_CMDRDY_bm) == 0);
		COUNT_NOW = COUNTER + TCD0.CAPTUREA;
		COUNT_DISPLAY = COUNT_NOW - COUNT_PREVIOUS;
		COUNT_DISPLAY = COUNT_DISPLAY * 10;
		COUNT_PREVIOUS = COUNT_NOW;
		COUNT_NEW = 1;
	}
	
	RTC.INTFLAGS = 0x11; // interrupt handled
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
	RTC.CTRLA = RTC_PRESCALER_DIV1_gc | RTC_RTCEN_bm;
	RTC.PER = 1250-1; // 8kHz
	RTC.INTCTRL = RTC_OVF_bm;
	RTC.CLKSEL = RTC_CLKSEL_XTAL32K_gc; // clock in XOSC23K pin
}

void setup_TCB_PWM(){
	PORTA.DIRSET = PIN2_bm;
	TCB0.CTRLA |= TCB_ENABLE_bm; // Enable the timer
	TCB0.CTRLB |= TCB_CCMPEN_bm; // Output on PA2 (pin 32)
	TCB0.CTRLB |= TCB_CNTMODE_PWM8_gc; // 8-bit PWM mode
	TCB0.CCMPL = 255; // period
	TCB0.CCMPH = 50; // duty
}

void setup_button(){
	PORTF.DIRCLR = PIN5_bm; // pin 25
	PORTF.PIN5CTRL = PORT_PULLUPEN_bm;
}

void wait_for_button_press(){
	while(PORTF.IN & PIN5_bm){}
}

int main(void)
{
	setup_system_clock_24MHz();
	setup_serial();
	setup_led();
	setup_extclk_counter();
	setup_rtc_gate();
	setup_TCB_PWM();
	setup_button();
	
	sei(); // Enable global interrupts
	
	printf("\r\nSTARTING...\r\n");
	
	while (1){
		wait_for_button_press();
		led_on();
		speak_mhz(COUNT_DISPLAY, 3);
		led_off();
	}
}
