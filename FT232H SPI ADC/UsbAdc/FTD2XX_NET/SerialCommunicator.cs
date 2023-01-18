using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static FTD2XX_NET.FTDI;
using static System.Windows.Forms.VisualStyles.VisualStyleElement;

namespace UsbAdc.FTD2XX_NET;

public class SerialCommunicator : CommunicatorBase
{

    public void SetBaudRate(int baudRate = 9600)
    {
        Status = FtdiDevice.SetBaudRate((uint)baudRate);
        AssertOK();
    }
}
