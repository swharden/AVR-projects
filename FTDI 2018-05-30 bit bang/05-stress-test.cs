using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using FTD2XX_NET;

namespace FTDI_video_demo
{
    class Program
    {
        public static FTDI ftdi = new FTDI();
        public static FTDI.FT_STATUS ft_status = FTDI.FT_STATUS.FT_OK;
        public static UInt32 bytesWritten = 0;

        public static byte pin_clock = 0b00000001;
        public static byte pin_data = 0b00000010;
        //public static byte pin_select = 0b00000001;

        static void Main(string[] args)
        {
            ft_status = ftdi.OpenByIndex(0);
            ft_status = ftdi.SetBaudRate(9600);
            ft_status = ftdi.SetBitMode(0b11111111, 1);

            // RIN, DCD, DSR, DTR, CTS, RTS, RXD, TXD
            List<byte> data = new List<byte>();

            for (int i = 0; i < 1000; i++){
                data.Add((byte)(i % 4));
            }

            int count = 0;
            while (true)
            {
                ft_status = ftdi.Write(data.ToArray(), data.Count, ref bytesWritten);
                System.Console.WriteLine($"{count++} {ft_status}");
                System.Threading.Thread.Sleep(100);
            }
        }
    }
}
