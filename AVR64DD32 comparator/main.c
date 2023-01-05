#include <avr/io.h>

uint8_t count = 0;
int main(void)
{
	// analog inputs: PD2 (pin12) and PD3 (pin13)
	PORTD.IN |= PIN2_bm | PIN3_bm;
	
	// comparator output: PA7 (pin5)
	PORTA.DIR |= PIN7_bm;
	
	// enable output pin and enable AC0
	AC0.CTRLA |= AC_OUTEN_bm | AC_ENABLE_bm;
	
	while (1)
	{
	}
}

