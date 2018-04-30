using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using FTD2XX_NET; // installed with NuGet

namespace FT232H_spi7seg
{
    class Program
    {

        const byte PIN_NONE = 0b00000000;
        const byte PIN_CLK  = 0b00000001; // TX
        const byte PIN_CS   = 0b00000010; // RX
        //const byte PIN_DATA = 0b00000100;
        const byte PIN_DATA = 0b00001000; // CTS

        public static FTDI ftdi = new FTDI();
        public static FTDI.FT_STATUS ft_status = FTDI.FT_STATUS.FT_OK;
        public static UInt32 bytesWritten = 0;

        public static List<byte> data = new List<byte>(); // bytes to send


        static void Main(string[] args)
        {

            ftdi.OpenByIndex(0);
            ftdi.SetBitMode(0, 0);
            byte mask = PIN_CLK | PIN_CS | PIN_DATA; // output pins
            ftdi.SetBitMode(mask, 0x01); // Asynchronous Bit Bang Mode
            ftdi.SetBaudRate(3_000_000/16);
            //ftdi.SetBaudRate(300);

            DisplayClear();
            SendCommand(0x0C, 0x01); // shutdown mode: normal operation
            SendCommand(0x09, 0xff); // decode mode: BCD for all digits
            SendCommand(0x0A, 0x0f); // intensity: brightest (last byte)
            SendCommand(0x0B, 0x07); // scan limit (number of rows in matrix)

            DisplayClear();

            System.Diagnostics.Stopwatch stopwatch = System.Diagnostics.Stopwatch.StartNew();

            System.Console.WriteLine("displaying time...");
            while (true)
            {
                double timeMS = stopwatch.ElapsedTicks * 1000.0 / System.Diagnostics.Stopwatch.Frequency;
                System.Console.WriteLine(string.Format("{0:0.000}", timeMS/100));
                DisplayNumber((long)timeMS, -3);
            }

            //ftdi.Close();
            //System.Console.WriteLine("DONE");
        }

        static void DisplayClear()
        {
            for(int i=1; i<=8; i++) SendCommand((byte)i, (byte)0x0F);
            BufferSend();
        }
        
        static void DisplayNumber(long val, int decimalPlace=6)
        {
            if (decimalPlace < 0) decimalPlace = 8 + decimalPlace;
            long divby = 10000000;
            for (int i=1; i<=8; i++)
            {
                byte charValue = (byte)(val / divby);
                byte charAddress = (byte)i;
                bool decimalHere = (i == decimalPlace);
                SetCharacter(charAddress, charValue, decimalHere);
                val -= (val / divby) * divby;
                divby /= 10;
            }
        }

        static void SetCharacter(int position, int value, bool decimalPoint = false)
        {
            position = 9 - position;
            if (decimalPoint) value |= 0b10000000;
            SendCommand((byte)position, (byte)value);
            BufferSend();
        }

        static void SendCommand(byte address, byte value)
        {
            data.Clear();
            BufferAddByte(address);
            BufferAddByte(value);
            BufferSend();
        }

        static void BufferAddByte(byte value)
        {
            //Console.Write("{0:X} ", value);
            for (int j = 0; j < 8; j++)
            {
                byte thisValue = 0;
                if ((value >> (7 - j) & 0b1) > 0) thisValue = PIN_DATA;
                data.Add(thisValue); // load the value (and lower clock)
                data.Add((byte)(thisValue | PIN_CLK)); // then raise clock
            }
        }
        
        static void BufferSend()
        {
            data.Insert(0, PIN_CS);
            data.Add(PIN_CS);
            ftdi.Write(data.ToArray(), data.Count, ref bytesWritten);
            data.Clear();
        }
        
    }
}
