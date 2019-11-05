import serial
import time

ser1 = serial.Serial('/dev/ttyACM0', 57600)

def getRelevantData(line):
    vraceni = ""
    for x in line:
        if  (x == "("):
            break
        vraceni += x
    return vraceni

soubor = open('spiral2.gcode', 'r')
gcode = soubor.readlines()

time.sleep(1)
while True:
    for line in gcode:
        data = getRelevantData(line)
        if data != "":
            #print data
            ser1.write(data)
            while True:
                serVstup = ser1.readline()
                #print serVstup
                if serVstup[0] == "b":
                    break;