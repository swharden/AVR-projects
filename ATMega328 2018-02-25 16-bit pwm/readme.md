# 16-bit PWM on ATMega328

## Quickstart

```C
void SetupPWM(){
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
    for(;;){}
}
```

## Interrupts

```C

ISR(TIMER1_COMPA_vect){
    // Timer/Counter1 Compare Match for when: TIMSK1|=(1<<OCIE1A)
    // this does not produce 50% duy
    //DDRB|=(1<<PB0); // ensure outupt
    //PORTB^=(1<<PB0); // flip its state
}


ISR(TIMER1_OVF_vect){
    // Timer/Counter1 Overflow for when: TIMSK1|=(1<<TOIE1)
    // this produces 50% duty
    DDRB|=(1<<PB0); // ensure outupt
    PORTB^=(1<<PB0); // flip its state
}

void SetupPWM(){
	// 16-bit timer timer for 100 Hz, ~200 us pulses (when OCR1A = 500) with an 8 MHz clock
	DDRB|=(1<<PB1); // 16-bit PWM output on PB1
	TCCR1A|=(1<<COM1A1); // Clear OC1A/OC1B on Compare Match when upcounting
	TCCR1B|=(1<<WGM13); // enable "PWM, phase and frequency correct"
	TCCR1B|=(1<<CS10); // enable output, fastest clock (no prescaling)
	ICR1=40000; // set the top value (up to 2^16)
	OCR1A=0; // set PWM pulse width (duty)
	//TIMSK1|=(1<<OCIE1A); // timer A compare match interrupt
    TIMSK1|=(1<<TOIE1); // overflow interrupt
	sei();
}

int main()
{
    DDRB|=(1<<PB1); // set PWM pin as output
    PORTB|=(1<<PB1); // pull high on start-up
    _delay_ms(500); // wait for soft start-up
    PORTB&=~(1<<PB1); // pull low like usual
    _delay_ms(500); // wait for soft start-up
    
    SetupPWM();
    OCR1A=500; // set this value to set PWM
    for(;;){}
}
```
