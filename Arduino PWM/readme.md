Arduino Fast PWM Mode

```c
void setup() {
  // see ATMega328P datasheet Section 14.9

  // set PWM pin output
  pinMode(PIN_POWER, HIGH);

  // PWM output pin A. clear OC0A on compare match. Fast PWM mode 3.
  TCCR0A = bit(COM0A1) | bit(WGM01) | bit(WGM00); 

  // Clock at CPU rate without prescaling.
  TCCR0B = bit(CS00);
}

volatile char brightness = 0;

void loop() {
  OCR0A = brightness++;
  _delay_ms(1);
}
```