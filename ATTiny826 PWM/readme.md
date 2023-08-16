# ATTiny826 PWM

## 16-bit PWM Output (TCA)

```c
#define F_CPU 3333333UL
#include <avr/io.h>
#include <util/delay.h>

void setup_TCA_singleSlope(){
	/* Generate PWM waveform on WO0 (pin 11)
	 * Frequency is defined by period (PER)
	 * Level is defined by comparator (CMP0)
	 */
	
	// enable this peripheral
	TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;
	
	// Override the value on WO0 (pin 11)
	TCA0.SINGLE.CTRLB |= TCA_SINGLE_CMP0EN_bm;
	
	// Use single slope PWM mode
	TCA0.SINGLE.CTRLB |= TCA_SINGLE_WGMODE_SINGLESLOPE_gc;
	
	TCA0.SINGLE.PER = 256; // top value
	TCA0.SINGLE.CMP0 = 150; // flip value
}

int main(void)
{
	PORTA.DIR = 0xFF;
	PORTB.DIR = 0xFF;
	
	setup_TCA_singleSlope();
	
	while (1)
	{
		_delay_ms(500);
		TCA0.SINGLE.CMP0 = 50;
		_delay_ms(500);
		TCA0.SINGLE.CMP0 = 200;
	}
}
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
