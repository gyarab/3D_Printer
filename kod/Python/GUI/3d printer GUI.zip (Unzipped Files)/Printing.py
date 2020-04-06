import struct
import time
import threading
import serial
from tkinter import *

class Printing(threading.Thread):
    def __init__(self, path, device, progress_bar, output_log, Percentage_Label, Time_Label, btn_stop, btn_end, btn_getfile, hystory_list):
        threading.Thread.__init__(self)
        self.device = device.split(" ")[0]

        self.start_time = time.time()
        self.printing = False
        self.Pause = True
        self.End = False

        self.btnStop = btn_stop
        self.btnEnd = btn_end
        self.btnGetfile = btn_getfile
        self.hystoryList = hystory_list

        self.progressbar = progress_bar
        self.outputLog = output_log
        self.PercentageLabel = Percentage_Label
        self.TimeLabel = Time_Label

        self.gcodeFile = open(path, 'r')
        self.gcode = self.gcodeFile.readlines()
        self.gcodeFile.close()

        self.length = len(self.gcode)
        self.currentLine = 0
        self.commandLog = []

    def getTime(self):
        vraceni = ""
        seconds = int(time.time()-self.start_time)

        if seconds//3600 > 0:
            vraceni += str(seconds//3600)+":"
        seconds = seconds%3600
        if (seconds//60 > 0):
            vraceni += str(seconds//60)+":"
        seconds = seconds%60
        vraceni += str(seconds)
        return vraceni

    def getRelevantData(self, line):
        vraceni = ""
        for x in line:
            if (x == "("):
                break
            vraceni += x
        return vraceni

    def getPercentage(self):
        return (self.currentLine*100/self.length)

    def afterEnd(self):
        self.hystoryList.config(state=NORMAL)
        self.btnGetfile.config(state=NORMAL)
        self.btnEnd.config(state=DISABLED)
        self.End = True
        self.ser1.close()

    def output(self, line = False):
        self.progressbar['value'] = self.getPercentage()
        self.PercentageLabel.config(text=str(int(self.getPercentage()))+"%")
        self.TimeLabel.config(text=self.getTime())
        if (line != False):
            self.commandLog.append(line)
            self.outputLog.delete(1.0, "end")
            if len(self.commandLog) > 20:
                del (self.commandLog[0])
            for x in self.commandLog:
                self.outputLog.insert("end", x)

    def run(self):
        self.ser1 = serial.Serial(self.device, 115200)
        time.sleep(2)
        self.printing = True
        for line in self.gcode:
            self.btnEnd.config(state=NORMAL)
            self.hystoryList.config(state=DISABLED)
            self.btnGetfile.config(state=DISABLED)
            data = self.getRelevantData(line)

            while (self.Pause == True):
                self.output()
                if (self.End == True):
                    break
            if (self.End == True):
                break

            if data != "" and data != "\n":
                self.ser1.write(struct.pack('>B',(len(data))))
                self.ser1.write(data.encode())
                self.output(line)
                while True:

                    if (self.ser1.in_waiting > 0):
                        serVstup = self.ser1.readline().decode('utf-8')
                        if serVstup[0] == "b":
                            break;
                        else:
                            pass
                    if (self.End == True):
                        break
                    self.output()

            self.currentLine += 1
        self.output("\n")
        self.output("konec")
        self.afterEnd()
        return (0)