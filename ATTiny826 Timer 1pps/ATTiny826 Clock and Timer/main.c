#include <avr/io.h>
#include <avr/interrupt.h>
/* Perform a Configuration Change Protection (CCP) write */
void ccp_write(volatile register8_t* address, uint8_t value){
	CCP = CCP_IOREG_gc;
	*address = value;
}

void configure_clock_internal_10mhz(){
	CCP = CCP_IOREG_gc;
	ccp_write(&CLKCTRL.MCLKCTRLA, CLKCTRL.MCLKCTRLA | CLKCTRL_CLKOUT_bm); // 20 MHz internal clock, enable CKOUT
	ccp_write(&CLKCTRL.MCLKCTRLB, CLKCTRL_PEN_bm); // enable divide-by-2 clock prescaler
}

void configure_clock_external(){
	CCP = CCP_IOREG_gc;
	ccp_write(&CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_EXTCLK_gc | CLKCTRL_CLKOUT_bm); // external clock, enable CKOUT
	ccp_write(&CLKCTRL.MCLKCTRLB, 0); // disable prescaler
}

void configure_1pps(){
	// 10 MHz system clock with div64 prescaler is 156,250 Hz.
	// Setting a 16-bit timer's top to 31,250 means 5 overflows per second.
	TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm; // overflow interrupt
	TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_NORMAL_gc; // normal mode
	TCA0.SINGLE.PER = 31250UL; // timer period
	TCA0.SINGLE.CTRLA |= TCA_SINGLE_CLKSEL_DIV64_gc; // set clock source
	TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm; // start timer}

volatile uint8_t overflow_count = 0; //

ISR(TCA0_OVF_vect) // called 10 times per second
{
	overflow_count++;
	if (overflow_count == 5){
		overflow_count = 0;
		PORTB.OUT = PIN1_bm;
		} else {
		PORTB.OUT = 0;
	}
	
	TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm; // indicate interrupt was handled}
int main(void)
{
	PORTB.DIR |= PIN1_bm; // set LED pin as output
	sei(); // enable global interrupts
	
	configure_clock_internal_10mhz();
	//configure_clock_external();
	configure_1pps();
	while(1){}
}