# ftdiDDS.exe

**FtdiDDS.exe is a console application created to easly set the frequency of an AD9850 RF frequency synthesizer by bit-banging SPI messages using an FT-232 (or similar) FTDI chip.** This program does _not_ require any special drivers. Just hook up the output pins of the FTDI chip (TX, RX, and CTS) to the AD9850 (clock, data, and enable, respectively) and run the command!

### Example Usage

* `ftdiDDS -list` _lists all available FTDI devices_
* `ftdiDDS -mhz 12.34` _sets frequency to 12.34 MHz_
* `ftdiDDS -device 2 -mhz 12.34` _specifically control device 2_
* `ftdiDDS -sweep` _sweep 0-50 MHz over 5 seconds_
* `ftdiDDS -help` _shows all options including a wiring diagram_

### Compiled Program
Although the Visual Studio code is provided in the source folder, the zip contains just the compiled program and supportive libraries. It is ready to run with no additional setup.
