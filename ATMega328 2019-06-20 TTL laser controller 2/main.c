/* ATMega328P - LED BLINK */

#define F_CPU 1000000ul
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	DDRC = 255; // all PORTC is output
	DDRD = 0;   // all PORTD is input

	for (;;)
	{
		if ((PIND & (1 << PD0)))
		{
			// pin is high
			PORTC |= (1<<PC5);
		}
		else
		{
			// pin is low
			PORTC &= ~(1<<PC5);
		}
	}
}
