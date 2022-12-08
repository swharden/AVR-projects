#define F_CPU 3333333UL // 20 MHz with /6 prescaler (datasheet 11.3.3)
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	PORTA.DIR = 0xFF;
	while (1)
	{
		PORTA.OUT = 255;
		_delay_ms(500);
		PORTA.OUT = 0;
		_delay_ms(500);
	}
}

