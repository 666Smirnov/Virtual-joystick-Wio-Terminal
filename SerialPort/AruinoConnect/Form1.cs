using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Globalization;
using System.IO.Ports;
//using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
//using System.Threading.Tasks;
using System.Windows.Forms;
using static System.Windows.Forms.VisualStyles.VisualStyleElement;
using vJoyInterfaceWrap;





namespace AruinoConnect
{
    class SignalFilter
    {
        private float[] buffer;
        private int bufferSize;
        private int currentIndex;

        public SignalFilter(int bufferSize)
        {
            this.bufferSize = bufferSize;
            buffer = new float[bufferSize];
            currentIndex = 0;
        }

        public void RawData(float data)
        {
            buffer[currentIndex] = data;
            currentIndex = (currentIndex + 1) % bufferSize;
        }

        public float FilterSignal()
        {
            float sum = 0;
            for (int i = 0; i < bufferSize; i++)
            {
                sum += buffer[i];
            }
            return (float) Math.Round(sum / bufferSize, 2);
        }
    }
    public partial class Form1 : Form
    {
        bool isConnected = false;
        float X;
        float Y;
        float Z;

        int joyX;
        int joyY;

        private vJoy joystick;
        private uint id = 1;

        string[] XYZ = new string[3];
        public Form1()
        {
            InitializeComponent();
            // Инициализация vJoy
            joystick = new vJoy();
            joystick.AcquireVJD(id);
        }

        private void arduinoButton_Click(object sender, EventArgs e)
        {
            comboBox1.Items.Clear();
            // Получаем список COM портов доступных в системе
            string[] portnames = SerialPort.GetPortNames();
            // Проверяем есть ли доступные
            if (portnames.Length == 0)
            {
                MessageBox.Show("COM PORT not found");
            }
            Console.WriteLine(portnames.Length);
            foreach (string portName in portnames)
            {
                //добавляем доступные COM порты в список           
                comboBox1.Items.Add(portName);
                if (portnames[0] != null)
                {
                    comboBox1.SelectedItem = portnames[0];
                }
            }
        }
        private void connectToArduino()
        {
            
            isConnected = true;
            string selectedPort = comboBox1.GetItemText(comboBox1.SelectedItem);
            serialPort = new SerialPort(selectedPort, 115200, Parity.None, 8, StopBits.One);
            serialPort.DtrEnable = true;
            serialPort.Open();
            button2.Text = "Disconnect";

            if (serialPort.IsOpen) { Console.WriteLine("Порт открыт"); }
            serialPort.DataReceived += SerialPort_DataReceived;
             
        }
        private void SerialPort_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            string data = serialPort.ReadExisting();
            // Здесь вы можете обрабатывать полученные данные
            //Console.WriteLine(data);
            XYZ = data.Split(',');
            X = float.Parse(XYZ[0], CultureInfo.InvariantCulture);
            Y = float.Parse(XYZ[1], CultureInfo.InvariantCulture);
            Z = float.Parse(XYZ[2], CultureInfo.InvariantCulture);
            serialPort.DiscardInBuffer();

            SignalFilter filterX = new SignalFilter(3);
            SignalFilter filterY = new SignalFilter(3);
            SignalFilter filterZ = new SignalFilter(3);

            filterX.RawData(X);
            filterY.RawData(Y);
            filterZ.RawData(Z);
            //Console.WriteLine(filterX.FilterSignal());
            //Console.WriteLine(filterY.FilterSignal());
            //Console.WriteLine(filterZ.FilterSignal());

            joystick.SetAxis((int)(X * 16500 + 16500), id, HID_USAGES.HID_USAGE_X);
            joystick.SetAxis((int)(Y * 16500 + 16500), id, HID_USAGES.HID_USAGE_Y);
            //joystick.SetAxis((int)(Z * 16000 + 16000), id, HID_USAGES.HID_USAGE_Z);


            label1.Invoke((MethodInvoker)delegate
            {
                label1.Text = XYZ[0];
                label2.Text = XYZ[1];
                label3.Text = XYZ[2];
            });
        }


        private void disconnectFromArduino()
        {
            isConnected = false;
            serialPort.Close();
            button2.Text = "Connect";
        }
        private void connectButton_Click(object sender, EventArgs e)
        {
            if (!isConnected)
            {
                connectToArduino();
            }
            else
            {
                disconnectFromArduino();
            }
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            // При закрытии программы, закрываем порт
            if (serialPort.IsOpen) serialPort.Close();
        }
    }
}
