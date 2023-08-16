# ATTiny826 PWM

TCA is a lot "smarter" and has more features than TCB, but both can do many similar things

## TCA 16-bit PWM Output

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

## TCA 16-bit PWM Interrupts

```c
#include <avr/interrupt.h>
```

```c
// enable this peripheral
TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;

// Overflow triggers interrupt
TCA0.SINGLE.INTCTRL |= TCA_SINGLE_OVF_bm;

// Set period and duty
TCA0.SINGLE.PER = 2500; // 8 kHz at 20 mHz clock

sei(); // enable global interrupts
```

```c
ISR(TCA0_OVF_vect)
{
	/* do something */
	TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm; // indicate interrupt was handled
}
```

## TCB 8-bit PWM Output

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

## TCB 16-bit PWM Interrupts

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
	TCB0.INTFLAGS = TCB_OVF_bm; // indicate interrupt was handled
}
```
