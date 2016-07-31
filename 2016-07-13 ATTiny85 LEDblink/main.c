#define	F_CPU (8000000UL)
#include <avr/io.h>
#include <util/delay.h>

int main (void)
{
    DDRB = 255; 
    while(1) 
    {
        PORTB ^= 255;
        _delay_ms(500);
    }
}