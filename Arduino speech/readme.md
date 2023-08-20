# Arduino Speech

Additional information is on https://github.com/swharden/NumberSpeaker

```c
const uint8_t AUDIO_SAMPLES[] PROGMEM = { 189, 160, 183, ... 128, 128, 128 };

void setup() {
  // Use timer 2 to generate an analog voltage using PWM on pin 11
  pinMode(11, OUTPUT);
  TCCR2A |= bit(COM2A1);
  TCCR2B = bit(CS20);

  // Setup status LED
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {

  digitalWrite(LED_BUILTIN, HIGH);
  for (int i = 0; i < sizeof(AUDIO_SAMPLES); i++) {
    uint8_t value = pgm_read_byte(&AUDIO_SAMPLES[i]);
    OCR2A = value;
    delayMicroseconds(200);
  }
  digitalWrite(LED_BUILTIN, LOW);

  for (;;) {}
}
```