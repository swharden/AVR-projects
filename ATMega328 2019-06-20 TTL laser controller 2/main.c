/* 
 * ATMega328P - 4 channel laser controller
 * 
 * Read inputs on pins 2-5 (PD0-PD3)
 *  - inputs are TTL from an opto-isolated high-voltage source (with H11A1)
 *  - input runs through a 10K series resistor (2.8 mA at 28V)
 * 
 * Produce output on pins 28-25 (PC5-PC2) if corresponding inputs are high.
 *  - output is a modulated signal: 25 ms period (40 Hz) with 5 ms high.
 * 
 */

#define F_CPU 11059200ul // 1MHz internal RC clock
#include <avr/io.h>
#include <util/delay.h>

// set these to control the waveform
const char msHigh = 5;
const char msLow = 20;

void initializePorts()
{
	DDRC = 255; // all PORTC is output
	DDRD = 0;   // all PORTD is input
}

void setOutputState(char channel, char state)
{
	char channelBit;
	if (channel == 1)
		channelBit = PC5;
	else if (channel == 2)
		channelBit = PC4;
	else if (channel == 3)
		channelBit = PC3;
	else if (channel == 4)
		channelBit = PC2;

	if (state == 1)
		PORTC |= (1 << channelBit);
	else if (state == 0)
		PORTC &= ~(1 << channelBit);
}

char isHigh(char channel)
{
	char channelBit;
	if (channel == 1)
		channelBit = PD0;
	else if (channel == 2)
		channelBit = PD1;
	else if (channel == 3)
		channelBit = PD2;
	else if (channel == 4)
		channelBit = PD3;

	char pinState = (PIND & (1 << channelBit));
	return pinState;
}

volatile char msRemainingCh1 = 0;
volatile char msRemainingCh2 = 0;
volatile char msRemainingCh3 = 0;
volatile char msRemainingCh4 = 0;

void updateAllMsRemaining()
{
	if (isHigh(1) && (msRemainingCh1 == 0))
		msRemainingCh1 = msHigh + msLow - 1;
	if (isHigh(2) && (msRemainingCh2 == 0))
		msRemainingCh2 = msHigh + msLow - 1;
	if (isHigh(3) && (msRemainingCh3 == 0))
		msRemainingCh3 = msHigh + msLow - 1;
	if (isHigh(4) && (msRemainingCh4 == 0))
		msRemainingCh4 = msHigh + msLow - 1;
}

void updateStateByMsRemaining()
{
	if (msRemainingCh1 >= msLow)
		setOutputState(1, 1);
	else
		setOutputState(1, 0);

	if (msRemainingCh2 >= msLow)
		setOutputState(2, 1);
	else
		setOutputState(2, 0);

	if (msRemainingCh3 >= msLow)
		setOutputState(3, 1);
	else
		setOutputState(3, 0);

	if (msRemainingCh4 >= msLow)
		setOutputState(4, 1);
	else
		setOutputState(4, 0);
}

void decrementMsRemaining()
{
	if (msRemainingCh1)
		msRemainingCh1--;
	if (msRemainingCh2)
		msRemainingCh2--;
	if (msRemainingCh3)
		msRemainingCh3--;
	if (msRemainingCh4)
		msRemainingCh4--;
}

void respondToInputsForever()
{
	for (;;)
	{
		updateAllMsRemaining();
		updateStateByMsRemaining();
		_delay_ms(1);
		_delay_us(4); // tweak with o-scope to improve timing
		decrementMsRemaining();
	}
}

void testLights(int times)
{
	// turn every output on and off several times in sequence
	// don't keep this in production or it will turn on the laser
	while (times--)
	{
		char i;
		for (i = 1; i <= 4; i++)
		{
			setOutputState(i, 1);
			_delay_ms(20);
		}
		for (i = 1; i <= 4; i++)
		{
			setOutputState(i, 0);
			_delay_ms(20);
		}
	}
}

int main(void)
{
	initializePorts();
	//testLights(10);
	respondToInputsForever();
}
