#!/usr/bin/env python
# -*- coding: utf-8 -*-


from tkinter import *
import tkinter as tk
from tkinter import ttk
from tkinter.filedialog import askopenfilename

import serial
from serial.tools import list_ports

from Printing import Printing



Printer = False
import os
import cssutils
import logging
cssutils.log.setLevel(logging.CRITICAL)

mode = "normie_mode.css"
device = ""

def getDctFromCss(path):
    dct = {}
    sheet = cssutils.parseFile(path)
    for rule in sheet:
        for prop in rule.style:
            dct[prop.name] = prop.value
    return dct

style = getDctFromCss(mode)

bgColor = style['bg-color']
fontNadpis = (style['font'], style['nadpis-size'])
fontText = (style['font'], style['text-size'])
textColor = style['text-color']
fieldColor = style['field-color']

btnColor = style['btn-color']
btnHighlightColor = style['btn-highlight-color']
btnHighlightThickness = style['btn-highlight-thickness']
btnHighlightBackground = style['btn-highlight-background']

history = []
selected = ""

def onselect(evt):
    global Printer
    global selected
    w = evt.widget
    index = int(w.curselection()[0])
    selected = history[len(history)-index-1]
    selectedLabel.config( text="..."+selected[len(selected)-25:len(selected)-1],
                          font=(fontText, 10), fg=style['text-color'], bg=style['bg-color'])
    btn_end.config(state=NORMAL)
    btn_pause.config(state=NORMAL)

def showHistory():
    hisoryList.delete(0, END)
    for line in reversed(history):
        hisoryList.insert(END, "..." + line[len(line) - 25:len(line)])


def readHistory():
    global history
    history = []
    f = open("history.txt", "r")
    for line in f:
        history.append(line[:-1])
    f.close()

def writeHistory(path):
    global history
    readHistory()
    if path != "":
        for x in range(len(history)):
            if history[x] == path:
                del (history[x])
                break
        history.append(path)
    if len(history) > 30:
        del(history[0])
    open('history.txt', 'w').close()
    f = open("history.txt", "a")
    for x in history:
        f.write(x+"\r")
    f.close()

def changeMode():
    global mode
    global cislo
    if mode == "normie_mode.css":
        mode = "dark_mode.css"
    else:
        mode = "normie_mode.css"
    refresh()

def getfile():
    global Printer
    global selected
    filename = askopenfilename(filetypes=[("gcode files", "*.gcode")])
    writeHistory(filename)
    readHistory()
    showHistory()
    hisoryList.selection_set(0,0)
    selected = history[len(history)-1]
    print(selected)
    selectedLabel.config( text="..."+selected[len(selected)-25:len(selected)-1],
                          font=(fontText, 10), fg=textColor, bg=bgColor)
    btn_end.config(state=NORMAL)
    btn_pause.config(state=NORMAL)

def end():
    #btn_end.config(state=DISABLED)
    #hisoryList.config(state=NORMAL)
    Printer.End = True




def pause():
    global Printer
    global selected
    btn_end.config(state=NORMAL)
    if Printer == False:
        Printer = Printing(selected, device, bar, textView, PercentageLabel, TimeLabel, btn_pause, btn_end, btn_getfile,
                           hisoryList)
    if Printer.End == True:
        Printer.End = True
        Printer = Printing(selected, device, bar, textView, PercentageLabel, TimeLabel, btn_pause, btn_end, btn_getfile, hisoryList)
    if (Printer.printing == False):
        hisoryList.config(state=DISABLED)
        btn_getfile.config(state=DISABLED)
        Printer.start()

    if Printer.Pause == False:
        Printer.Pause = True
    else:
        Printer.Pause = False



window = Tk()


barFrame = tk.Frame(window)
barFrame.grid(column=2, row=1, padx=style['padx'], rowspan=2)

PercentageLabel = Label(barFrame, text="", font=fontText, fg=textColor, bg=bgColor)
PercentageLabel.pack(side=TOP)

bar = ttk.Progressbar(barFrame, length=300)
bar.pack()

TimeLabel = Label(barFrame, text="", font=fontText, fg=textColor, bg=bgColor)
TimeLabel.pack(side=BOTTOM)

frame = tk.Frame(window)

scrollbar = Scrollbar(frame, orient="vertical", width=15)

frame.grid(column=0, row=5, rowspan=10, columnspan=2)
historyLabel = Label(frame, text="history", font=fontText, fg=textColor, bg=bgColor)
historyLabel.pack(side=TOP)

scrollbar.pack(side=RIGHT, fill=Y)
hisoryList = Listbox(frame, height=20, width=30)
hisoryList.bind('<<ListboxSelect>>', onselect)
hisoryList.pack(side=LEFT, fill=Y, )
hisoryList.config(yscrollcommand=scrollbar.set)
scrollbar.config(command=hisoryList.yview)

textViewFrame = tk.Frame(window)

selectedLabel = Label(textViewFrame)
selectedLabel.pack(side=TOP)

textView = Text(textViewFrame, height=20, width=50)
textView.pack(side=BOTTOM, fill=BOTH)
textViewFrame.grid(column=2, row=5, columnspan=2, pady=(30,0))




choices = []
for x in serial.tools.list_ports.comports(include_links=False):
    choices.append(x)

if len(choices) == 0:
    choices.append("no device found")

tkvar = StringVar(window)
tkvar.set(choices[0])
device = tkvar.get()

def change_dropdown(*args):
    global device
    global Printer
    device = tkvar.get()

tkvar.trace('w', change_dropdown)
popupMenu = OptionMenu(window, tkvar, *choices)
popupMenu.config(width = 35)
popupMenu.grid(column=1, row=1,  padx=style['padx'], pady=style['pady'])

btn_end = Button(window, state=DISABLED)
btn_end.grid(column=3, row=2, sticky=W, padx=style['padx'], pady=0)

btn_pause = Button(window, state=DISABLED)
btn_pause.grid(column=3, row=1, sticky=W, padx=style['padx'], pady=0)

btn_mode = Button(window)
btn_mode.grid(column=4, row=0, padx=style['padx'], pady=style['pady'])


btn_getfile = Button(window)
btn_getfile.grid(column=1, row=0, sticky="w", padx=style['padx'], pady=style['pady'])

def vp_start_gui(cssFile = "normie_mode.css"):
    global window
    global endIcon
    global pauseIcon
    global style
    style = getDctFromCss(cssFile)

    window.title("Bohemian Print")

    bgColor = style['bg-color']
    fontNadpis = (style['font'], style['nadpis-size'])
    fontText = (style['font'], style['text-size'])
    textColor = style['text-color']
    fieldColor = style['field-color']


    btnColor = style['btn-color']
    btnHighlightColor = style['btn-highlight-color']
    btnHighlightThickness = style['btn-highlight-thickness']
    btnHighlightBackground = style['btn-highlight-background']

    window.configure(bg=bgColor)


    lbl = Label(window, text="Bohemian Print GUI", font=fontNadpis, fg=textColor, bg=bgColor)
    lbl.grid(column=2, row=0, padx=4)

    endIcon = PhotoImage(file =style['end-icon']+".png")
    pauseIcon = PhotoImage(file =style['pause-icon']+".png")
    endIcon = endIcon.subsample(16)
    pauseIcon = pauseIcon.subsample(16)

    btn_getfile.config(text="Vybrat soubor tisku", font=fontText, bg=btnColor, fg=textColor,command=getfile,
                 highlightcolor = btnHighlightColor, highlightbackground = btnHighlightBackground, highlightthickness = btnHighlightThickness)

    btn_end.config(text = "bagr", font=fontText, bg=btnColor, fg=textColor, command=end, image = endIcon,
                 highlightcolor = btnHighlightColor, highlightbackground = btnHighlightBackground, highlightthickness = btnHighlightThickness)

    btn_pause.config(font=fontText, bg=btnColor, fg=textColor, command=pause, image = pauseIcon,
                 highlightcolor = btnHighlightColor, highlightbackground = btnHighlightBackground, highlightthickness = btnHighlightThickness)

    btn_mode.config(font=fontText, bg=btnColor, fg=textColor, command=changeMode, image = pauseIcon,
                 highlightcolor = btnHighlightColor, highlightbackground = btnHighlightBackground, highlightthickness = btnHighlightThickness)

    ttk.Style().configure("red.Horizontal.TProgressbar", troughcolor=fieldColor, background=btnColor)
    bar.config(style="red.Horizontal.TProgressbar")

    popupMenu.config(bg=btnColor, fg=textColor, highlightcolor = btnHighlightColor, highlightbackground = btnHighlightBackground)

    selectedLabel.config(font=(fontText, 10), fg=textColor, bg=bgColor)
    historyLabel.config(font=(fontText, 10), fg=textColor, bg=bgColor)
    PercentageLabel.config(font=(fontText, 10), fg=textColor, bg=bgColor)
    TimeLabel.config(font=(fontText, 10), fg=textColor, bg=bgColor)

    frame.config(bg=bgColor)
    textViewFrame.config(bg=bgColor)
    barFrame.config(bg=bgColor)

    hisoryList.config(bg=fieldColor, fg=textColor)
    textView.config(bg=fieldColor, fg=textColor)
    scrollbar.config(bg = fieldColor)

    readHistory()
    showHistory()


if __name__ == '__main__':

    window.geometry("800x600")
    vp_start_gui(cssFile=mode)

    def refresh():
        vp_start_gui(cssFile=mode)

    window.mainloop()


