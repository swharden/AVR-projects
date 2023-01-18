using FTD2XX_NET;

namespace UsbAdc.FTD2XX_NET;

public abstract class CommunicatorBase
{
    protected FTDI FtdiDevice = new();
    protected FTDI.FT_STATUS Status = FTDI.FT_STATUS.FT_OK;

    public FTDI.FT_DEVICE_INFO_NODE[] Scan()
    {
        UInt32 numDevices = 0;
        FtdiDevice.GetNumberOfDevices(ref numDevices);
        FTDI.FT_DEVICE_INFO_NODE[] devices = new FTDI.FT_DEVICE_INFO_NODE[numDevices];
        FtdiDevice.GetDeviceList(devices);
        return devices;
    }

    public void OpenByIndex(int index = 0)
    {
        Status = FtdiDevice.OpenByIndex((uint)index);
        AssertOK();
    }

    protected void AssertOK()
    {
        if (Status != FTDI.FT_STATUS.FT_OK)
            throw new InvalidOperationException(Status.ToString());
    }

    protected void Write(byte[] bytes)
    {
        uint bytesWritten = 0;
        Status = FtdiDevice.Write(bytes, bytes.Length, ref bytesWritten);
        AssertOK();
    }

    protected byte[] ReadBytes(uint count)
    {
        byte[] readBuffer = new byte[count];
        uint bytesRead = 0;
        Status = FtdiDevice.Read(readBuffer, count, ref bytesRead);
        AssertOK();
        return readBuffer;
    }

    protected void FlushBuffer()
    {
        uint BytesAvailable = 0;
        Status = FtdiDevice.GetRxBytesAvailable(ref BytesAvailable);
        AssertOK();

        if (BytesAvailable == 0)
            return;

        byte[] buffer = new byte[BytesAvailable];
        uint NumBytesRead = 0;
        Status = FtdiDevice.Read(buffer, BytesAvailable, ref NumBytesRead);
        AssertOK();
    }
}
