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
volatile int NEXTPWM=0;
volatile int ticks=0; // % doesn't work with char
ISR(TIMER1_COMPA_vect){
	PORTB=NEXTB; // do this first so there's no delay due to ifs
	OCR0A=NEXTPWM; // do this for consistency
	wdt_reset(); // reset watchdog
	
	// make pretty pulsating LED
	if (ticks<=100) {NEXTPWM=ticks;}
	else {NEXTPWM=200-ticks;}
	// convert [0-100] to [0-255] but don't let it get to 0
	NEXTPWM=NEXTPWM*225/100+30;

	ticks++;
	if (ticks==200){ticks=0;} // resets every second
	if (ticks%10==0){NEXTB^=(1<<PB1);} // 20Hz
	if (ticks%100==0){NEXTB^=(1<<PB3);PORTD^=(1<<PD6);} // 1Hz 50% duty
	if (ticks<10){NEXTB|=(1<<PB0);} else {NEXTB&=~(1<<PB0);} // 1Hz blip
	
}

void timer1_setup(){
	/* set up 16-bit timer for 200Hz events at 10Mhz using CTC mode */
	TCCR1B|=(1<<WGM12); // CTC mode with OCR1A top
	OCR1A=50000; // set the top
	TCCR1B|=(1<<CS10); // use internal clock with no prescaling
	TIMSK|=(1<<OCIE1A); // fire interrupt when compare match occurs
}

void timer0_setup(){
	/* set up 8-bit timer for fast PWM output on OC0A (PB2) */
	TCCR0A|=(1<<COM0A1); // Clear OC0A on Compare Match
	TCCR0A|=(1<<WGM02)|(1<<WGM01)|(1<<WGM00); // PWM mode
	TCCR0B|=(1<<CS00); // use internal clock with no prescaling
}
	
int main(void){
	WDTCSR|=(1<<WDP2)|(1<<WDP1);
	DDRB|=(1<<PB0);
	DDRB|=(1<<PB1);
	DDRB|=(1<<PB2);
	DDRB|=(1<<PB3);
	DDRD|=(1<<PD6);
	timer1_setup(); // 10MHz to 200Hz
	timer0_setup(); // pwm
	sei(); // enable global interrupts
	while(1){} // without this, program terminates and interrupts stop
}
