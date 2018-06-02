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
            int freqTarget = 12_345_678; // Hz
            while (true)
            {
                List<byte> bytesToSend = new List<byte>();

                freqTarget += 1;
                ulong freqCode = (ulong)(freqTarget) * (ulong)4_294_967_296;
                ulong freqCrystal = 125_000_000;
                freqCode = freqCode / freqCrystal;
                
                // load the target frequency
                bytesToSend.Add(ReverseBits((byte)((freqCode >> 00) & 0xFF))); // 1 LSB
                bytesToSend.Add(ReverseBits((byte)((freqCode >> 08) & 0xFF))); // 2
                bytesToSend.Add(ReverseBits((byte)((freqCode >> 16) & 0xFF))); // 3
                bytesToSend.Add(ReverseBits((byte)((freqCode >> 24) & 0xFF))); // 4 MSB
                bytesToSend.Add(0);

                BitBangBytes(bytesToSend.ToArray());
                System.Console.WriteLine($"{count++} {ft_status} {freqTarget}");
                System.Threading.Thread.Sleep(20);
            }
        }
        
        /// <summary>
        /// return a byte with all of its bits in reverse order
        /// </summary>
        public static byte ReverseBits(byte b1)
        {
            byte b2 = 0;
            for (int i=0; i<8; i++)
            {
                b2 += (byte)(((b1 >> i) & 1) << 7 - i);
            }
            return b2;
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
            // pulse enable to clear what was there before
            states.Insert(0, pin_enable);
            states.Insert(0, 0);

            // pulse enable to apply configuration
            states.Add(pin_enable);
            states.Add(0);
            ft_status = ftdi.Write(states.ToArray(), states.Count, ref bytesWritten);
        }
    }
}
