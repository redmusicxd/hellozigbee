import serial

ser = serial.Serial(
    baudrate=115200,\
    parity=serial.PARITY_NONE,\
    stopbits=serial.STOPBITS_ONE,\
    bytesize=serial.EIGHTBITS,\
    timeout=None)

ser.dtr = True
ser.rts = True
ser.port = '/dev/ttyUSB0'

ser.open()

ser.close()

ser.dtr = True
ser.rts = False

ser.open()

ser.close()

