#define F_CPU 24000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

int main(void)
{
	CCP = CCP_IOREG_gc; // Protected write
	CLKCTRL.OSCHFCTRLA = CLKCTRL_FRQSEL_24M_gc; // Set clock to 24MHz
	
	PORTD.DIR = 0xFF;

	while (1)
	{
		_delay_ms(500);
		PORTD.OUT = 255;
		_delay_ms(500);
		PORTD.OUT = 0;
	}
}
