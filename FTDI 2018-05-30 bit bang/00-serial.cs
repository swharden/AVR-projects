using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using FTD2XX_NET;

namespace FTDIstate
{
    class Program
    {
        public static FTDI ftdi = new FTDI();
        public static FTDI.FT_STATUS ft_status = FTDI.FT_STATUS.FT_OK;
        public static UInt32 bytesWritten = 0;
        
        static void Main(string[] args)
        {
            ft_status = ftdi.OpenByIndex(0);
            ft_status = ftdi.SetBaudRate(9600);
			string data = "a";
			ft_status = ftdi.Write(data, data.Length, ref bytesWritten);
        }
    }
}
