# ADC on ATTiny2313

## 16-bit ADC output on PB3
```C
void Setup16BitADC(){
  // set up the timer to produce 100 Hz, 200 us pulses (when OCR1A = 500) with an 8 MHz clock
  DDRB|=(1<<PB3); // 16-bit PWM output on PB3
	TCCR1A|=(1<<COM1A1); // Clear OC1A/OC1B on Compare Match when upcounting
	TCCR1B|=(1<<WGM13); // enable "PWM, phase and frequency correct"
	TCCR1B|=(1<<CS10); // enable output, fastest clock (no prescaling)
	ICR1=40000; // set the top value (up to 2^16)
	OCR1A=0; // set PWM pulse width (duty)
}
```
