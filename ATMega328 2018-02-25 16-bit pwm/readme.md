# 16-bit PWM on ATMega328

```C
void SetupV1(){
	// 16-bit timer timer for 100 Hz, ~200 us pulses (when OCR1A = 500) with an 8 MHz clock
	DDRB|=(1<<PB1); // 16-bit PWM output on PB1
	TCCR1A|=(1<<COM1A1); // Clear OC1A/OC1B on Compare Match when upcounting
	TCCR1B|=(1<<WGM13); // enable "PWM, phase and frequency correct"
	TCCR1B|=(1<<CS10); // enable output, fastest clock (no prescaling)
	ICR1=40000; // set the top value (up to 2^16)
	OCR1A=0; // set PWM pulse width (duty)
}

int main()
{
    SetupPWM();
    OCR1A=500; // set this value to set PWM
}
```

## Overflow Interrupt

```C

//ISR(TIMER1_OVF_vect){ // do this at the end of each 100Hz pulse
ISR(TIMER1_COMPA_vect){
    tick+=1;
    if (tick==ticksOn) {
        OCR1A=0;
    }
    if (tick>ticksOn+ticksOff){
        tick=0;
        OCR1A=pulseWidth;
    }    
}

void SetupV2(){
	// 16-bit timer timer for 100 Hz, ~200 us pulses (when OCR1A = 500) with an 8 MHz clock
	DDRB|=(1<<PB1); // 16-bit PWM output on PB1
	TCCR1A|=(1<<COM1A1); // Clear OC1A/OC1B on Compare Match when upcounting
	TCCR1B|=(1<<WGM13); // enable "PWM, phase and frequency correct"
	TCCR1B|=(1<<CS10); // enable output, fastest clock (no prescaling)
	ICR1=40000; // set the top value (up to 2^16)
	OCR1A=500; // set PWM pulse width (duty)
	//TIMSK1|=(1<<TOIE1); // timer A overflow interrupt
	TIMSK1|=(1<<OCIE1A); // timer A compare match interrupt
	sei();
}
```
