
void SpiInitialize(void)
{
	DDRB |= (1 << PB5);  // set SCK as output
	DDRB |= (1 << PB3);  // set MOSI as output
	DDRB |= (1 << PB2);  // set CS as output
	PORTB |= (1 << PB2); // set CS high

	SPCR |= (1 << SPE);  // enable SPI
	SPCR |= (1 << MSTR); // this device is SPI master
	SPCR |= (1 << SPR0); // clock prescale osc/16
}

void SpiSend(unsigned char data)
{
	SPDR = data;
	while (!(SPSR & (1 << SPIF)))
	{
	};
}

void DisplaySendCommand(uint8_t address, uint8_t data)
{
	PORTB &= ~(1 << PB2); // set CS low
	SpiSend(address);
	SpiSend(data);
	PORTB |= (1 << PB2); // set CS high
}

void DisplayNumber(long number)
{
	for (int i = 1; i <= 8; i++)
	{
		char thisDigit = number % 10;
		if (i == 4)
			thisDigit |= 0xf0; // add decimal point
		if (i == 7)
			thisDigit |= 0xf0; // add decimal point
		DisplaySendCommand(i, thisDigit);
		number /= 10;
	}
}

void DisplayInitialize()
{
	DisplaySendCommand(0x0C, 0x01); // shutdown mode: normal operation
	DisplaySendCommand(0x09, 0xff); // decode mode: BCD for all digits
	DisplaySendCommand(0x0A, 0x0f); // intensity: brightest (last byte)
	DisplaySendCommand(0x0B, 0x07); // display size: max character address
}

void DisplayClear()
{
	for (int i = 1; i <= 8; i++)
		DisplaySendCommand(i, 0x0F);
}