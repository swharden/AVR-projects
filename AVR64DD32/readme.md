# AVR64DD32 

## Pinout

![](avr64dd32-pinout.png)

## LED Blink

```c
#define F_CPU 24000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

int main(void)
{
	CCP = CCP_IOREG_gc; // Protected write
	CLKCTRL.OSCHFCTRLA = CLKCTRL_FRQSEL_24M_gc; // Set clock to 24MHz
	
	PORTD.DIR = 0xFF;

	while (1)
	{
		_delay_ms(500);
		PORTD.OUT = 255;
		_delay_ms(500);
		PORTD.OUT = 0;
	}
}
```

## Datasheet

* https://www.mouser.com/datasheet/2/268/AVR64DD32_28_Prelim_DataSheet_DS40002315B-2950084.pdf

## TCB0 16-bit counter

```c
ISR(TCB0_INT_vect)
{
	TIMER_OVERFLOWS++;
	if (TIMER_OVERFLOWS == 20){
		TIMER_OVERFLOWS = 0;
		TIMER_READY = 1;
	}
	TCB0.INTFLAGS = TCB_OVF_bm | TCB_CAPT_bm;
}
```

```c
// Setup TCB to interrupt periodically
TCB0.CTRLA |= TCB_CLKSEL_DIV2_gc; // use peripheral clock / 2
TCB0.INTCTRL |= TCB_CAPT_bm;
TCB0.CCMP = 60000;
TCB0.CTRLA |= TCB_ENABLE_bm; // start the counter

sei();
```
