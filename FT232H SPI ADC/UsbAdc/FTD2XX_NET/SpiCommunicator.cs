/* This file was tested using a MCP3201 SPI ADC */

namespace UsbAdc.FTD2XX_NET;

internal class SpiCommunicator : CommunicatorBase
{
    public void SetupSPI()
    {
        FtdiDevice.ResetDevice();
        AssertOK();
        Status = FtdiDevice.SetBitMode(0, 0); // reset
        AssertOK();
        Status = FtdiDevice.SetBitMode(0, 0x02); // MPSSE 
        AssertOK();
        Status = FtdiDevice.SetLatency(16);
        AssertOK();
        Status = FtdiDevice.SetTimeouts(1000, 1000); // long
        AssertOK();
        Thread.Sleep(50);

        // Configure the MPSSE for SPI communication
        byte[] bytes1 = new byte[]
        {
            0x8A, // disable clock divide by 5 for 60Mhz master clock
            0x97, // turn off adaptive clocking
            0x8d // disable 3 phase data clock
        };
        Write(bytes1);

        // The SK clock frequency can be worked out by below algorithm with divide by 5 set as off
        // SCL Frequency (MHz) = 60 / ((1 + DIVISOR) * 2)
        UInt32 clockDivisor = 29; // for 1 MHz

        byte[] bytes2 = new byte[]
        {
            0x80, // Set directions of lower 8 pins and force value on bits set as output
            0x00, // Set SDA, SCL high, WP disabled by SK, DO at bit ＆＊, GPIOL0 at bit ＆＊
            0x0b, // Set SK,DO,GPIOL0 pins as output with bit ＊, other pins as input with bit ＆＊
            0x86, // use clock divisor
            (byte)(clockDivisor & 0xFF), // clock divisor low byte
            (byte)(clockDivisor >> 8), // clock divisor high byte
        };
        Write(bytes2);
        Thread.Sleep(50);

        // disable loopback
        Write(new byte[] { 0x85 });
        Thread.Sleep(50);
    }

    public int ReadAdc()
    {
        CsLow();

        byte[] writeBuffer =
        {
            0x24, // MSB_FALLING_EDGE_CLOCK_BYTE_IN
            0x01, // two bytes
            0x00,
        };
        Write(writeBuffer);

        byte[] readBuffer = new byte[2];
        uint bytesRead = 0;
        Status = FtdiDevice.Read(readBuffer, 2, ref bytesRead);
        AssertOK();

        CsHigh();

        byte b1 = (byte)(readBuffer[0] & 0b00011111); // see MCP3201 datasheet figure 6-1
        byte b2 = (byte)(readBuffer[1] & 0b11111110);
        int value = b1 * 256 + b2;

        return value;
    }

    public void CsHigh()
    {
        byte[] bytes = new byte[]
        {
            0x80, // GPIO command for ADBUS
            0x08, // set CS high, MOSI and SCL low
            0x0b, // bit3:CS, bit2:MISO, bit1:MOSI, bit0:SCK
        };
        Write(bytes);
    }

    public void CsLow()
    {
        byte[] bytes = new byte[]
        {
            0x80, // GPIO command for ADBUS
            0x00, // set CS, MOSI, and SCL all low
            0x0b, // bit3:CS, bit2:MISO, bit1:MOSI, bit0:SCK
        };
        Write(bytes);
    }
}
