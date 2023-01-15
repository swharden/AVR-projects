namespace UsbAdc;

public partial class Form1 : Form
{
    readonly FtdiManager FTMan = new();

    public Form1()
    {
        InitializeComponent();
    }

    private void Form1_Load(object sender, EventArgs e)
    {
        foreach(var device in FTMan.Scan())
            cbDevices.Items.Add($"{device.Type} ({device.ID})");

        if (cbDevices.Items.Count > 0)
            cbDevices.SelectedIndex = 0;
    }

    private void btnOpen_Click(object sender, EventArgs e)
    {
        FTMan.OpenByIndex(cbDevices.SelectedIndex);
    }
}
