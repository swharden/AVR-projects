#define F_CPU 10000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

// clock source is EXT and delivered into PA0
// CKOUT is enabled so square waves come out PD2
// watchdog is also enabled for ~1s if the 200Hz isn't called.
// this is set with the fuse [WDTON=0]

volatile char NEXTB=0;
volatile int ticks=0; // % doesn't work with char

ISR(TIMER1_COMPA_vect){
	PORTB=NEXTB; // do this first so there's no delay due to ifs
	wdt_reset(); // reset watchdog
	ticks++;
	if (ticks==200){ticks=0;} // resets every second
	if (ticks%100==0){NEXTB^=(1<<PB3);} // 1Hz 50% duty	
}

void timer1_setup(){
	/* set up 16-bit timer for 200Hz events at 10Mhz using CTC mode */
	TCCR1B|=(1<<WGM12); // CTC mode with OCR1A top
	OCR1A=50000; // set the top
	TCCR1B|=(1<<CS10); // use internal clock with no prescaling
	TIMSK|=(1<<OCIE1A); // fire interrupt when compare match occurs
}
	
int main(void){
	WDTCSR|=(1<<WDP2)|(1<<WDP1);
	timer1_setup(); // 10MHz to 200Hz
	sei(); // enable global interrupts
	while(1){} // without this, program terminates and interrupts stop
}
