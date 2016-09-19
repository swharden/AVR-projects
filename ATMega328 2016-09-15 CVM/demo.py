import serial
import serial.tools.list_ports

for potentialPort in list(serial.tools.list_ports.comports()):
    print(potentialPort.device,"-",potentialPort.description)

ser = serial.Serial()
ser.port = "COM19"
ser.baudrate = 9600
ser.bytesize = serial.EIGHTBITS
ser.parity = serial.PARITY_NONE
ser.stopbits = serial.STOPBITS_ONE
ser.timeout = 1.5
ser.open()
try:
    while True:
        print(ser.readline().decode("utf-8").strip())
except Exception as ER:
    print("EXCEPTION")
    print(ER)
ser.close()
print("DONE")