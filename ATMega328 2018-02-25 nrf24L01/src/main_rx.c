/*
* ----------------------------------------------------------------------------
* “THE COFFEEWARE LICENSE” (Revision 1):
* <ihsan@kehribar.me> wrote this file. As long as you retain this notice you
* can do whatever you want with this stuff. If we meet some day, and you think
* this stuff is worth it, you can buy me a coffee in return.
* -----------------------------------------------------------------------------
*/

#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <util/delay.h>
#include "radioPinFunctions.c"
#include "nrf24.h"
#include "nrf24.c"

// system variables
volatile uint8_t tick=0;
volatile unsigned int ticksSinceLastRadioSignal=0;
uint8_t temp;
uint8_t q = 0;
uint8_t data_array[4] = {1,2,3,4}; // ticksOn, ticksOff, pulseWidthHighByte, pulseWidthLowByte
uint8_t address_A[5] = {0xE7,0xE7,0xE7,0xE7,0xE7}; // will be radio-to-PC
uint8_t address_B[5] = {0xD7,0xD7,0xD7,0xD7,0xD7}; // will be PC-to-radio

void LEDblink(){
    PORTB|=(1<<PB1);
    _delay_ms(50);
    PORTB&=~(1<<PB1);
    _delay_ms(50);
}

// low-level stimulation modulation
volatile uint8_t ticksOn=1;
volatile uint8_t ticksOff=19;
volatile unsigned int pulseWidth = 500; //OCR1A

ISR(TIMER1_OVF_vect){
    // do this at the end of each 100Hz pulse
    tick+=1;
    DDRB&=~(1<<PB1);
    if (tick==ticksOn) {
        OCR1A=0;
    } else {
        DDRB|=(1<<PB1);
    }
    if (tick>=ticksOn+ticksOff){
        tick=0;
        OCR1A=pulseWidth;
    }    
    ticksSinceLastRadioSignal+=1;
}

void SetupStimulator(){
	// 16-bit timer timer for 100 Hz, ~200 us pulses (when OCR1A = 500) with an 8 MHz clock
	DDRB|=(1<<PB1); // 16-bit PWM output on PB3
	TCCR1A|=(1<<COM1A1); // Clear OC1A/OC1B on Compare Match when upcounting
	TCCR1B|=(1<<WGM13); // enable "PWM, phase and frequency correct"
	TCCR1B|=(1<<CS10); // enable output, fastest clock (no prescaling)
	ICR1=40000; // set the top value (up to 2^16)
	OCR1A=500; // set PWM pulse width (duty)
    TIMSK1|=(1<<TOIE1); // timer A overflow interrupt
    sei();
}

void SetupRadio(){
    nrf24_init();
    nrf24_config(2,4); // Channel #2 , payload length: 4
    nrf24_tx_address(address_A);
    nrf24_rx_address(address_B);   
}

void SetupIO(){
    DDRB|=(1<<PB1); // set PB1 as output (pin 15) for stimulator
    DDRD|=(1<<PD7); // red LED
    DDRD|=(1<<PD6); // green LED
}

void LedNone(){
    PORTD&=~(1<<PD7); // red off
    PORTD&=~(1<<PD6); // green off
}

void LedGreen(){
    LedNone();
    PORTD|=(1<<PD6); // green on
}

void LedRed(){
    LedNone();
    PORTD|=(1<<PD7); // red on
}


int main()
{
    SetupIO();
    SetupRadio();
    SetupStimulator();

    for(;;){         
        if(nrf24_dataReady())
        {
            nrf24_getData(data_array);
            // check if an error message was setnt 
            if (data_array[0]==0 && data_array[1]==0 && data_array[2]==0 && data_array[3]==0){
                LedRed();
            } else {
                LedGreen();
                ticksOn = data_array[0];
                ticksOff = data_array[1];
                pulseWidth = data_array[2]<<8 + data_array[3];
            }
        }
    }
}