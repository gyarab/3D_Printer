import serial
import struct
import time

ser1 = serial.Serial('/dev/ttyACM0', 57600)


vstup = ["G02 X40.0 Y00.0 I20.0 J00.0 Z0.0",
         "G02 X00.0 Y00.0 I-20.0 J00.0 Z0.0"]

#vstup = ["G01 X0.0 Y0.0 Z0.0"]
#vstup = ["G01 X-50.0 Y-50.0 Z0.0"]
#vstup = ["G01 X0.0 Y-30.0 Z0.0"]

time.sleep(1)
while True:
    for x in vstup:

        ser1.write(x)
        while True:
            serVstup = ser1.readline()
            print serVstup
            if serVstup[0] == "b":
                break

    #raw_input()

