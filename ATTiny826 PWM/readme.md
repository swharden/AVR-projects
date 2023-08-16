# ATTiny826 PWM

## 16-bit PWM Output (TCA)

```c
// enable this peripheral
TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;

// Override the value on WO0 (pin 11)
TCA0.SINGLE.CTRLB |= TCA_SINGLE_CMP0EN_bm;

// Use single slope PWM mode
TCA0.SINGLE.CTRLB |= TCA_SINGLE_WGMODE_SINGLESLOPE_gc;

// Set period and duty
TCA0.SINGLE.PER = 1234; // top value
TCA0.SINGLE.CMP0 = 123; // flip value
```

## 8-bit PWM Output (TCB)

```c
// Enable this peripheral
TCB0.CTRLA |= TCB_ENABLE_bm;

// Make waveform output available on the pin
TCB0.CTRLB |= TCB_CCMPEN_bm;

// Enable 8-bit PWM mode
TCB0.CTRLB |= TCB_CNTMODE_PWM8_gc;

// Set period and duty
TCB0.CCMPL = 255; // top value
TCB0.CCMPH = 100; // flip value
```

## 16-bit PWM Interrupts (TCB)

```c
#include <avr/interrupt.h>
```

```c
TCB0.CTRLA |= TCB_ENABLE_bm; // Enable this peripheral
TCB0.INTCTRL |= TCB_OVF_bm; // Trigger interrupt on overflow
sei(); // enable global interrupts
```

```c
ISR(TCB0_INT_vect)
{
    /* do something */
}
```
