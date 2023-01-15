using System.Diagnostics;

namespace UsbAdc;

public partial class Form1 : Form
{
    readonly FtdiManager FTMan = new();
    readonly Stopwatch SW = new();
    double MaxSeenValue = 0;

    public Form1()
    {
        InitializeComponent();
    }

    private void Form1_Load(object sender, EventArgs e)
    {
        foreach (var device in FTMan.Scan())
            cbDevices.Items.Add($"{device.Type} ({device.ID})");

        if (cbDevices.Items.Count > 0)
            cbDevices.SelectedIndex = 0;

        btnOpen_Click(null!, EventArgs.Empty);
    }

    private void btnOpen_Click(object sender, EventArgs e)
    {
        cbDevices.Enabled = false;
        btnOpen.Enabled = false;
        FTMan.OpenByIndex(cbDevices.SelectedIndex);
        FTMan.SetupAdc();
        timer1.Enabled = true;
        SW.Restart();
    }

    private void timer1_Tick(object sender, EventArgs e)
    {
        double value = FTMan.ReadAdc();
        label1.Text = $"{value}";
        MaxSeenValue = Math.Max(MaxSeenValue, value);

        pnlLevel.Location = new(0, 0);
        pnlLevel.Height = pnlContainer.Height;
        pnlLevel.Width = (int)(pnlContainer.Width * (value / MaxSeenValue));
    }
}
