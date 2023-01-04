#define F_CPU 4000000UL
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	PORTD.DIR = 255;
	
    while (1) 
    {
		PORTD.OUT = 255;
		_delay_ms(100);
		PORTD.OUT = 0;
		_delay_ms(900);
    }
}

