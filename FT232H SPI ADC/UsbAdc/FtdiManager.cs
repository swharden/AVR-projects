using FTD2XX_NET;
using Microsoft.VisualBasic.Logging;
using static FTD2XX_NET.FTDI;

namespace UsbAdc;

internal class FtdiManager
{
    private FTDI FtdiDevice = new();
    private FTDI.FT_STATUS Status = FTDI.FT_STATUS.FT_OK;

    public void OpenByIndex(int index = 0)
    {
        Status = FtdiDevice.OpenByIndex((uint)index);
        AssertOK();
    }

    public void SetBaudRate(int baudRate = 9600)
    {
        Status = FtdiDevice.SetBaudRate((uint)baudRate);
        AssertOK();
    }

    public FTDI.FT_DEVICE_INFO_NODE[] Scan()
    {
        UInt32 numDevices = 0;
        FtdiDevice.GetNumberOfDevices(ref numDevices);
        FTDI.FT_DEVICE_INFO_NODE[] devices = new FTDI.FT_DEVICE_INFO_NODE[numDevices];
        FtdiDevice.GetDeviceList(devices);
        return devices;
    }

    private void AssertOK()
    {
        if (Status != FTDI.FT_STATUS.FT_OK)
            throw new InvalidOperationException(Status.ToString());
    }

    public void SetupAdc()
    {
        Status = FtdiDevice.SetLatency(16);
        const byte byte1 = 0x80; // GPIO command for ADBUS
        const byte byte2 = 0x08; // set CS high, MOSI and SCL 
        const byte byte3 = 0x0b; // bit3:CS, bit2:MISO, bit1: MOSI, bit0: SCK
        byte[] bytes = { byte1, byte2, byte3 };
        Write(bytes);
    }

    public void ReadAdc()
    {
        List<byte> bytes = new();

        // spi enable (5x to achieve 1us)
        for (int i = 0; i < 5; i++)
            bytes.AddRange(SpiEnableBytes());

        // read data

        // spi disable
        bytes.AddRange(SpiDisableBytes());
    }

    public void Write(byte[] bytes)
    {
        uint bytesWritten = 0;
        Status = FtdiDevice.Write(bytes, bytes.Length, ref bytesWritten);
        AssertOK();
    }

    byte[] SpiEnableBytes()
    {
        return new byte[]
        {
            0x80, // GPIO command for ADBUS
            0x08, // set CS high, MOSI and SCL low
            0x0b, // bit3:CS, bit2:MISO, bit1:MOSI, bit0:SCK
        };
    }

    byte[] SpiDisableBytes()
    {
        return new byte[]
        {
            0x80, // GPIO command for ADBUS
            0x00, // set CS, MOSI and SCL low
            0x0b, // bit3:CS, bit2:MISO, bit1:MOSI, bit0:SCK
        };
    }
}
