#define F_CPU 20000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <avr/interrupt.h>
void ccp_write(volatile register8_t* reg, uint8_t value){
	CCP = CCP_IOREG_gc;
	*reg = value;
}

void configure_clock(){
	CCP = CCP_IOREG_gc;
	ccp_write(&CLKCTRL.MCLKCTRLA, CLKCTRL.MCLKCTRLA | CLKCTRL_CLKOUT_bm); // 20 MHz internal clock, enable CKOUT
	ccp_write(&CLKCTRL.MCLKCTRLB, 0); // disable prescaler
}

void configure_1pps(){
	// 20 MHz system clock with div64 prescaler is 312,500 Hz.
	// Setting a 16-bit timer's period to 31,250 means 10 overflows per second.
	TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm; // overflow interrupt
	TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_NORMAL_gc; // normal mode
	TCA0.SINGLE.PER = 31250UL; // timer period
	TCA0.SINGLE.CTRLA |= TCA_SINGLE_CLKSEL_DIV64_gc; // set clock source
	TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm; // start timer}

volatile uint8_t overflow_count = 0; //

ISR(TCA0_OVF_vect) // called 10 times per second
{
	overflow_count++;
	if (overflow_count == 10){
		overflow_count = 0;
		PORTA.OUT = PIN4_bm;
		} else {
		PORTA.OUT = 0;
	}
	
	TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm; // indicate interrupt was handled}
int main(void)
{
	PORTA.DIR |= PIN4_bm; // set LED pin as output
	sei(); // enable global interrupts
	
	configure_clock();
	configure_1pps();
	while(1){}
}