Arduino Fast PWM Mode

## Timer 0 (8-bit)

```c
void setup_timer0() {
  // set PWM pin output
  pinMode(6, HIGH);

  // Set OC0A on BOTTOM, clear OC0A on compare match
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

## Timer 2 (8-bit)

```c
void setup_timer0() {
  // set PWM pin as output
  pinMode(11, OUTPUT);

  // Set OC2A on BOTTOM, clear OC2A on compare match
  TCCR2A |= bit(COM2A1);

  // Clock at CPU rate without prescaling.
  TCCR0B = bit(CS20);
}

volatile char brightness = 0;

void loop() {
  OCR2A = brightness++;
  _delay_ms(1);
}
```