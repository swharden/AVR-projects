#define F_CPU 14745600L
#include <avr/io.h>
#include <util/delay.h>

#include "display-max7219-7seg.c"

int main(void)
{
	SpiInitialize();
	long number;
	for (;;)
	{
		DisplayInitialize();
		DisplayNumber(number++);
		_delay_ms(10);
	}
}
