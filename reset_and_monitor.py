import serial

ser = serial.Serial(
    baudrate=115200,\
    parity=serial.PARITY_NONE,\
    stopbits=serial.STOPBITS_ONE,\
    bytesize=serial.EIGHTBITS,\
    timeout=None)

ser.dtr = False
ser.rts = True
ser.port = '/dev/ttyUSB0'

ser.open()

ser.close()

ser.rts = False

ser.open()

while True:
    try:
        cc=str(ser.readline())
        print(cc[2:][:-5])
        pass
    except KeyboardInterrupt:
        ser.close()
        break

