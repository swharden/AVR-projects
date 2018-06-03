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
            System.Console.WriteLine("\n");
            
            double freqMhz = 0;
            int freqHz = 0;
            uint deviceNumber = 0;
            bool goodArguments = true;
            bool sweep = false;

            if (args.Count() == 0) {
                ShowHelp();
                goodArguments = false;
            }

            for (var x = 0; x < args.Count(); x++)
            {
                string thisArgument = args[x].Trim().ToLower();
                string nextArgument = "";
                if (x < args.Count()-1) nextArgument = args[x+1].Trim().ToLower();


                if (thisArgument.ToCharArray()[0] == '-')
                {
                    switch (thisArgument)
                    {
                        case "-list":
                            ListDevices();
                            goodArguments = false;
                            break;
                        case "-mhz":
                            freqMhz = 0;
                            double.TryParse(nextArgument, out freqMhz);
                            freqHz = (int)(freqMhz * 1_000_000);
                            System.Console.WriteLine("Setting Frequency (Hz): " + string.Format("{0:n0}", freqHz));
                            break;
                        case "-hz":
                            freqHz = 0;
                            int.TryParse(nextArgument, out freqHz);
                            System.Console.WriteLine("Setting Frequency (Hz): " + string.Format("{0:n0}", freqHz));
                            break;
                        case "-device":
                            deviceNumber = 0;
                            uint.TryParse(nextArgument, out deviceNumber);
                            System.Console.WriteLine("Using FTDI Device: " + deviceNumber.ToString());
                            break;
                        case "-sweep":
                            System.Console.WriteLine("Sweeping Continuously...");
                            sweep = true;
                            break;
                        case "-help":
                            ShowHelp();
                            goodArguments = false;
                            break;
                        default:
                            System.Console.WriteLine("ERROR: Argument " + thisArgument);
                            goodArguments = false;
                            break;
                    }
                }
            }
            if (goodArguments == true && sweep == false) FrequencySetStable(deviceNumber, freqHz);
            if (goodArguments == true && sweep == true) FrequencySweep(deviceNumber);

        }

        public static void ListDevices()
        {
            uint nDevices = 0;
            ft_status = ftdi.GetNumberOfDevices(ref nDevices);
            FTDI.FT_DEVICE_INFO_NODE[] ftdiDeviceList = new FTDI.FT_DEVICE_INFO_NODE[nDevices];
            ft_status = ftdi.GetDeviceList(ftdiDeviceList);

            System.Console.WriteLine("  ATTACHED FTDI DEVICES:");


            for (int i = 0; i < nDevices; i++)
            {
                System.Console.WriteLine("\n    Device Number: " + i.ToString());
                System.Console.WriteLine("      Type: " + ftdiDeviceList[i].Type.ToString());
                System.Console.WriteLine("      Description: " + ftdiDeviceList[i].Description.ToString());
                System.Console.WriteLine("      Serial: " + ftdiDeviceList[i].SerialNumber.ToString());
            }
            System.Console.WriteLine("");


        }
        public static void ShowHelp(bool pause=true)
        {
            System.Console.WriteLine("");
            System.Console.WriteLine("  ftdiDDS.exe - Bit-Bang an AD9850 with an FTDI Chip");
            System.Console.WriteLine("                a software demo by Scott Harden");
            System.Console.WriteLine("");
            System.Console.WriteLine("  WIRING:");
            System.Console.WriteLine("    TX=CLK, RX=DATA, CTS=ENABLE");
            System.Console.WriteLine("    Tested with FT232R and FT232H");
            System.Console.WriteLine("");
            System.Console.WriteLine("  ARGUMENTS:");
            System.Console.WriteLine("    -list       list all attached FTDI devices");
            System.Console.WriteLine("    -device 3   Use a specific FTDI device (e.g., device 3)");
            System.Console.WriteLine("    -mhz 12.34  Go to a sepcific frequency (e.g., 12.34 MHz)");
            System.Console.WriteLine("    -hz 123456  Go to a sepcific frequency (e.g., 123456 Hz)");
            System.Console.WriteLine("    -sweep      Step through a range of frequencies (0-50 MHz)");
            System.Console.WriteLine("    -help       Display this message");
            System.Console.WriteLine("");
            if (pause)
            {
                System.Console.WriteLine("press ENTER to exit...");
                System.Console.ReadLine();
            }
        }

        /// <summary>
        /// open the FTDI device, sweep frequency forever
        /// </summary>
        public static void FrequencySweep(uint ftdiDevice = 0)
        {
            ft_status = ftdi.OpenByIndex(ftdiDevice);
            ft_status = ftdi.SetBaudRate(9600);
            ft_status = ftdi.SetBitMode(255, 1);
            while (true)
            {
                for (int i = 1; i <= 500; i++)
                {
                    FrequencySet(i * 1_000_00);
                    System.Threading.Thread.Sleep(10);
                }
            }
        }

        /// <summary>
        /// requires an already-open FTDI device
        /// </summary>
        public static void FrequencySet(int freqTarget = 10_000_000)
        {
            // determine the frequency code
            ulong freqCode = (ulong)(freqTarget) * (ulong)4_294_967_296;
            ulong freqCrystal = 125_000_000;
            freqCode = freqCode / freqCrystal;

            // load the target frequency
            List<byte> bytesToSend = new List<byte>();
            bytesToSend.Add(ReverseBits((byte)((freqCode >> 00) & 0xFF))); // 1 LSB
            bytesToSend.Add(ReverseBits((byte)((freqCode >> 08) & 0xFF))); // 2
            bytesToSend.Add(ReverseBits((byte)((freqCode >> 16) & 0xFF))); // 3
            bytesToSend.Add(ReverseBits((byte)((freqCode >> 24) & 0xFF))); // 4 MSB
            bytesToSend.Add(0);

            BitBangBytes(bytesToSend.ToArray());
            System.Console.WriteLine($"Set to " + string.Format("{0:n0}", freqTarget) + $" Hz - {ft_status}");
        }

        /// <summary>
        /// open the FTDI device, set frequency, then close the device
        /// </summary>
        public static void FrequencySetStable(uint ftdiDevice = 0, int freqTarget = 10_000_000)
        {
            ft_status = ftdi.OpenByIndex(ftdiDevice);
            ft_status = ftdi.SetBaudRate(9600);
            ft_status = ftdi.SetBitMode(255, 1);
            FrequencySet(freqTarget);
            ftdi.Close(); // detach cleanly;
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
