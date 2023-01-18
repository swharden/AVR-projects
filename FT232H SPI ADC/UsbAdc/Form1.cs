using System.Diagnostics;

namespace UsbAdc;

public partial class Form1 : Form
{
    readonly FTD2XX_NET.TwiCommunicator FTCOM = new();
    readonly Stopwatch SW = new();
    double MaxSeenValue = 0;
    int Readings = 0;

    public Form1()
    {
        InitializeComponent();
    }

    private void Form1_Load(object sender, EventArgs e)
    {
        foreach (var device in FTCOM.Scan())
            cbDevices.Items.Add($"{device.Type} ({device.ID})");

        if (cbDevices.Items.Count > 0)
            cbDevices.SelectedIndex = 0;

        btnOpen_Click(null!, EventArgs.Empty);
    }

    private void btnOpen_Click(object sender, EventArgs e)
    {
        cbDevices.Enabled = false;
        btnOpen.Enabled = false;
        FTCOM.OpenByIndex(cbDevices.SelectedIndex);
        FTCOM.I2C_ConfigureMpsse();
        timer1.Enabled = true;
        SW.Restart();
    }

    private void timer1_Tick(object sender, EventArgs e)
    {
        double value = FTCOM.I2C_ReadADC();
        Readings += 1;
        label1.Text = $"{value}";
        MaxSeenValue = Math.Max(MaxSeenValue, value);

        pnlLevel.Location = new(0, 0);
        pnlLevel.Height = pnlContainer.Height;
        pnlLevel.Width = (int)(pnlContainer.Width * (value / MaxSeenValue));

        Text = $"Read {Readings} in {SW.Elapsed.TotalSeconds:N2} sec " +
            $"({Readings / SW.Elapsed.TotalSeconds:N2} Hz)";
    }
}
