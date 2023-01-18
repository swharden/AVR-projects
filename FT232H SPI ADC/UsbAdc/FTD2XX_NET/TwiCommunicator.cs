/* This file was tested using a LM75A I2C temperature sensor */

using FTD2XX_NET;

namespace UsbAdc.FTD2XX_NET;
internal class TwiCommunicator : CommunicatorBase
{
    public void I2C_ConfigureMpsse()
    {
        Status |= FtdiDevice.SetTimeouts(1000, 1000);
        Status |= FtdiDevice.SetLatency(16);
        Status |= FtdiDevice.SetFlowControl(FTDI.FT_FLOW_CONTROL.FT_FLOW_RTS_CTS, 0x00, 0x00);
        Status |= FtdiDevice.SetBitMode(0x00, 0x00); // RESET
        Status |= FtdiDevice.SetBitMode(0x00, 0x02); // MPSSE
        AssertOK();

        FlushBuffer();

        /***** Synchronize the MPSSE interface by sending bad command 0xAA *****/
        Write(new byte[] { 0xAA });
        byte[] rx1 = ReadBytes(2);
        if ((rx1[0] != 0xFA) || (rx1[1] != 0xAA))
            throw new InvalidOperationException($"bad echo bytes: {rx1[0]} {rx1[1]}");

        /***** Synchronize the MPSSE interface by sending bad command 0xAB *****/
        Write(new byte[] { 0xAB });
        byte[] rx2 = ReadBytes(2);
        if ((rx2[0] != 0xFA) || (rx2[1] != 0xAB))
            throw new InvalidOperationException($"bad echo bytes: {rx2[0]} {rx2[1]}");

        const uint ClockDivisor = 49;
        const byte I2C_Data_SDAhi_SCLhi = 0x03;
        const byte I2C_Dir_SDAout_SCLout = 0x03;

        int numBytesToSend = 0;
        byte[] buffer = new byte[100];
        buffer[numBytesToSend++] = 0x8A;   // Disable clock divide by 5 for 60Mhz master clock
        buffer[numBytesToSend++] = 0x97;   // Turn off adaptive clocking
        buffer[numBytesToSend++] = 0x8C;   // Enable 3 phase data clock, used by I2C to allow data on both clock edges
                                           // The SK clock frequency can be worked out by below algorithm with divide by 5 set as off
                                           // SK frequency  = 60MHz /((1 +  [(1 +0xValueH*256) OR 0xValueL])*2)
        buffer[numBytesToSend++] = 0x86;   //Command to set clock divisor
        buffer[numBytesToSend++] = (byte)(ClockDivisor & 0x00FF);  //Set 0xValueL of clock divisor
        buffer[numBytesToSend++] = (byte)((ClockDivisor >> 8) & 0x00FF);   //Set 0xValueH of clock divisor
        buffer[numBytesToSend++] = 0x85;           // loopback off

        buffer[numBytesToSend++] = 0x9E;       //Enable the FT232H's drive-zero mode with the following enable mask...
        buffer[numBytesToSend++] = 0x07;       // ... Low byte (ADx) enables - bits 0, 1 and 2 and ... 
        buffer[numBytesToSend++] = 0x00;       //...High byte (ACx) enables - all off



        buffer[numBytesToSend++] = 0x80;   //Command to set directions of lower 8 pins and force value on bits set as output 
        buffer[numBytesToSend++] = I2C_Data_SDAhi_SCLhi;
        buffer[numBytesToSend++] = I2C_Dir_SDAout_SCLout;

        byte[] msg = buffer.Take(numBytesToSend).ToArray();
        Write(msg);
    }

    public void I2C_SetStart()
    {
        List<byte> bytes = new();

        const byte I2C_Data_SDAlo_SCLlo = 0x00;
        const byte I2C_Data_SDAlo_SCLhi = 0x01;
        const byte I2C_Data_SDAhi_SCLlo = 0x02;
        const byte I2C_Data_SDAhi_SCLhi = 0x03;

        const byte I2C_ADbus = 0x80;
        const byte I2C_Dir_SDAout_SCLout = 0x03;

        for (int i = 0; i < 6; i++)
            bytes.AddRange(new byte[] { I2C_ADbus, I2C_Data_SDAhi_SCLhi, I2C_Dir_SDAout_SCLout, });

        for (int i = 0; i < 6; i++)
            bytes.AddRange(new byte[] { I2C_ADbus, I2C_Data_SDAlo_SCLhi, I2C_Dir_SDAout_SCLout, });

        for (int i = 0; i < 6; i++)
            bytes.AddRange(new byte[] { I2C_ADbus, I2C_Data_SDAlo_SCLlo, I2C_Dir_SDAout_SCLout, });

        bytes.AddRange(new byte[] { I2C_ADbus, I2C_Data_SDAhi_SCLlo, I2C_Dir_SDAout_SCLout, });

        Write(bytes.ToArray());
    }

    public void I2C_SetStop()
    {
        List<byte> bytes = new();

        const byte I2C_Data_SDAlo_SCLlo = 0x00;
        const byte I2C_Data_SDAlo_SCLhi = 0x01;
        const byte I2C_Data_SDAhi_SCLhi = 0x03;

        const byte I2C_ADbus = 0x80;
        const byte I2C_Dir_SDAout_SCLout = 0x03;

        for (int i = 0; i < 6; i++)
            bytes.AddRange(new byte[] { I2C_ADbus, I2C_Data_SDAlo_SCLlo, I2C_Dir_SDAout_SCLout, });

        for (int i = 0; i < 6; i++)
            bytes.AddRange(new byte[] { I2C_ADbus, I2C_Data_SDAlo_SCLhi, I2C_Dir_SDAout_SCLout, });

        for (int i = 0; i < 6; i++)
            bytes.AddRange(new byte[] { I2C_ADbus, I2C_Data_SDAhi_SCLhi, I2C_Dir_SDAout_SCLout, });

        Write(bytes.ToArray());
    }

    public void I2C_SendDeviceAddr(byte address, bool read)
    {
        const byte I2C_Data_SDAhi_SCLlo = 0x02;
        const byte MSB_FALLING_EDGE_CLOCK_BYTE_OUT = 0x11;
        const byte I2C_Dir_SDAout_SCLout = 0x03;
        const byte MSB_RISING_EDGE_CLOCK_BIT_IN = 0x22;

        address <<= 1;
        if (read == true)
            address |= 0x01;

        byte[] buffer = new byte[100];
        int bytesToSend = 0;
        buffer[bytesToSend++] = MSB_FALLING_EDGE_CLOCK_BYTE_OUT;        // clock data byte out
        buffer[bytesToSend++] = 0x00;                                   // 
        buffer[bytesToSend++] = 0x00;                                   //  Data length of 0x0000 means 1 byte data to clock in
        buffer[bytesToSend++] = address;           //  Byte to send

        // Put line back to idle (data released, clock pulled low)
        buffer[bytesToSend++] = 0x80;                                   // Command - set low byte
        buffer[bytesToSend++] = I2C_Data_SDAhi_SCLlo;                               // Set the values
        buffer[bytesToSend++] = I2C_Dir_SDAout_SCLout;                               // Set the directions

        // CLOCK IN ACK
        buffer[bytesToSend++] = MSB_RISING_EDGE_CLOCK_BIT_IN;           // clock data bits in
        buffer[bytesToSend++] = 0x00;                                   // Length of 0 means 1 bit

        // This command then tells the MPSSE to send any results gathered (in this case the ack bit) back immediately
        buffer[bytesToSend++] = 0x87;                                //  ' Send answer back immediate command

        // send commands to chip
        byte[] msg = buffer.Take(bytesToSend).ToArray();
        Write(msg);

        byte[] rx1 = ReadBytes(1);

        // if ack bit is 0 then sensor acked the transfer, otherwise it nak'd the transfer
        if ((rx1[0] & 0x01) != 0)
            throw new InvalidOperationException("NAKd");
    }

    public byte I2C_ReadByte(bool ACK)
    {
        const byte MSB_RISING_EDGE_CLOCK_BYTE_IN = 0x20;
        const byte MSB_FALLING_EDGE_CLOCK_BIT_OUT = 0x13;
        const byte I2C_Data_SDAhi_SCLlo = 0x02;
        const byte I2C_Dir_SDAout_SCLout = 0x03;
        int bytesToSend = 0;

        // Clock in one data byte
        byte[] buffer = new byte[100];
        buffer[bytesToSend++] = MSB_RISING_EDGE_CLOCK_BYTE_IN;      // Clock data byte in
        buffer[bytesToSend++] = 0x00;
        buffer[bytesToSend++] = 0x00;                               // Data length of 0x0000 means 1 byte data to clock in

        // clock out one bit as ack/nak bit
        buffer[bytesToSend++] = MSB_FALLING_EDGE_CLOCK_BIT_OUT;     // Clock data bit out
        buffer[bytesToSend++] = 0x00;                               // Length of 0 means 1 bit
        if (ACK == true)
            buffer[bytesToSend++] = 0x00;                           // Data bit to send is a '0'
        else
            buffer[bytesToSend++] = 0xFF;                           // Data bit to send is a '1'

        // I2C lines back to idle state 
        buffer[bytesToSend++] = 0x80;                               //       ' Command - set low byte
        buffer[bytesToSend++] = I2C_Data_SDAhi_SCLlo;                            //      ' Set the values
        buffer[bytesToSend++] = I2C_Dir_SDAout_SCLout;                             //     ' Set the directions

        // This command then tells the MPSSE to send any results gathered back immediately
        buffer[bytesToSend++] = 0x87;                                  //    ' Send answer back immediate command

        // send commands to chip
        byte[] msg = buffer.Take(bytesToSend).ToArray();
        Write(msg);

        // get the byte which has been read from the driver's receive buffer
        byte[] readBuffer = new byte[1];
        uint bytesRead = 0;
        Status = FtdiDevice.Read(readBuffer, 1, ref bytesRead);
        AssertOK();

        return readBuffer[0];
    }

    public double I2C_ReadADC()
    {
        I2C_SetStart();
        I2C_SendDeviceAddr(0x48, read: true);
        byte b1 = I2C_ReadByte(ACK: false);
        byte b2 = I2C_ReadByte(ACK: false);
        I2C_SetStop();
        return (b1 * 256 + b2) / 32;
    }
}
