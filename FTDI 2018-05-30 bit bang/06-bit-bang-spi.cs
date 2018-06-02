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

        // RIN, DCD, DSR, DTR, CTS, RTS, RXD, TXD
        public static byte pin_clock = 0b00000001; // TX
        public static byte pin_data = 0b00000010; // RX
        public static byte pin_enable = 0b00001000; // CTS (active low)

        static void Main(string[] args)
        {
            ft_status = ftdi.OpenByIndex(0);
            ft_status = ftdi.SetBaudRate(9600);
            ft_status = ftdi.SetBitMode(255, 1); // all output, bit-bang
            
            int count = 0;
            while (true)
            {
                byte[] bytesToSend = { 222, 234, 246 };
                bytesToSend = Encoding.ASCII.GetBytes("Hello, World!");
                BitBangBytes(bytesToSend);
                System.Console.WriteLine($"{count++} {ft_status}");
                System.Threading.Thread.Sleep(100);
            }
        }
        
        public static List<byte> StatesFromByte(byte b)
        {
            List<byte> states = new List<byte>();
            for (int i=0; i<8; i++)
            {
                byte dataState = (byte)((b >> (7-i)) & 1); // 1 if this bit is high
                states.Add((byte)(pin_data * dataState)); // set data pin with clock low
                states.Add((byte)(pin_data * dataState | pin_clock)); // pull clock high
            }
            return states;
        }

        public static List<byte> StatesFromByte(byte[] b)
        {
            List<byte> states = new List<byte>();
            foreach (byte singleByte in b)
                states.AddRange(StatesFromByte(singleByte));
            return states;
        }

        public static void BitBangBytes(byte[] bytesToSend)
        {
            List<byte> states = StatesFromByte(bytesToSend);
            states.Insert(0, pin_enable);
            states.Add(pin_enable);
            ft_status = ftdi.Write(states.ToArray(), states.Count, ref bytesWritten);
        }
    }
}
