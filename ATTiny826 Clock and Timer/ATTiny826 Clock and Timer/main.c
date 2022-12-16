#define F_CPU 20000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>

void ccp_write(volatile register8_t* reg, uint8_t value){
	CCP = CCP_IOREG_gc;
	*reg = value;
}

void configure_clock(){
	ccp_write(&CLKCTRL.MCLKCTRLA, CLKCTRL.MCLKCTRLA | CLKCTRL_CLKOUT_bm); // 20 MHz internal clock, enable CKOUT
	ccp_write(&CLKCTRL.MCLKCTRLB, 0); // disable prescaler
}

int main(void)
{
	configure_clock();
	
	PORTA.DIR = 0xFF;
	while (1)
	{
		PORTA.OUT = 255;
		_delay_ms(500);
		PORTA.OUT = 0;
		_delay_ms(500);
	}
}