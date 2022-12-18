/* 
	This program demonstrates how to cascade the two 16-bit TCB counters to form a 32-bit counter. 
	The clock counted with TC1 and interrupts are used to produce a signal once per second to gate the counter.
*/
#define F_CPU 10000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "serial.h"

void ccp_write(volatile register8_t* address, uint8_t value){
	CCP = CCP_IOREG_gc;
	*address = value;
}

void configure_clock_external_10mhz(){
	CCP = CCP_IOREG_gc;
	ccp_write(&CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_EXTCLK_gc | CLKCTRL_CLKOUT_bm); // external clock, enable CKOUT
	ccp_write(&CLKCTRL.MCLKCTRLB, 0); // disable prescaler
}

void configure_1pps(){
	TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm; // overflow interrupt
	TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_NORMAL_gc; // normal mode
	TCA0.SINGLE.PER = 31249UL; // timer period (overflows every 200 ms)
	TCA0.SINGLE.CTRLA |= TCA_SINGLE_CLKSEL_DIV64_gc; // set clock source
	TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm; // start timer
}

void configure_cascaded_counter(){
	// Configure the event system to route TCB0 overflows to TCB1
	EVSYS.CHANNEL0 = EVSYS_CHANNEL0_TCB0_OVF_gc;
	EVSYS.USERTCB1COUNT = EVSYS.CHANNEL0;
	
	// Configure the event system to route the same capture event (CAPT) generator to both TCBs
	
	// configure timer B0 to count clock cycles
	TCB0.CTRLA = TCB_ENABLE_bm;
	TCB0.CTRLB = TCB_CNTMODE_CAPT_gc;
	
	// configure timer B1 to count events (timer 0 overflows)
	TCB1.CTRLA = TCB_CLKSEL_EVENT_gc | TCB_CASCADE_bm | TCB_ENABLE_bm;
	TCB1.CTRLB = TCB_CNTMODE_CAPT_gc;
}

unsigned long last_count = 0;
void send_current_count(){
	unsigned int lsb = TCB0.CNT;
	unsigned int msb = TCB1.CNT;
	unsigned long newCount = msb;
	newCount = newCount << 16;
	newCount += lsb;
	
	unsigned long diff = newCount - last_count;
	last_count = newCount;
	SERIAL_sendUnsignedLong(newCount);
	SERIAL_sendComma();
	SERIAL_sendUnsignedLong(diff);
	SERIAL_sendBreak();
}

uint8_t overflow_count;
ISR(TCA0_OVF_vect)
{
	overflow_count++;
	if (overflow_count == 5){
		overflow_count = 0;
		PORTB.OUT |= PIN1_bm;
		send_current_count();
		} else {
		PORTB.OUT &= ~PIN1_bm;
	}

	TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm; // indicate interrupt was handled
}

int reading = 0;
int main(void)
{
	configure_clock_external_10mhz();
	configure_1pps();
	USART0_init();
	configure_cascaded_counter();
	sei();
	PORTB.DIR |= PIN1_bm;
	while (1) {	}
}

