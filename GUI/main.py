from tkinter import Tk, W
from tkinter.ttk import Frame, Button, Label, Combobox, Notebook, Entry

import serial


def startButtonCallback():
    print("Start")


def commandButtonCallback():
    print("Send Command")
    print(initGUI.combo.get())
    if initGUI.combo.get() == "State":
        print(initGUI.stateEntry.get())
    elif initGUI.combo.get() == "RSSI Human to Zombie":
        print(initGUI.RSSIEntry.get())
    elif initGUI.combo.get() == "RSSI Zombie to Human":
        print(initGUI.RSSIEntry.get())
    else:
        pass

def idleButtonCallback():
    print("Idle")

def initGUI():

    tab1 = Frame(tab_control)
    tab2 = Frame(tab_control)

    tab_control.add(tab1, text = "Game Control")
    tab_control.add(tab2, text = "Settings")
    #tab_control.pack(expand = 1, fill = BOTH)
    tab_control.grid(row=0 , column=0, columnspan=5)

    startButton = Button(tab1, text="Start", command=startButtonCallback)
    #startButton.pack(side = LEFT, expand = True)
    startButton.grid(row=1, column=0, sticky=W)

    lbl1 = Label(tab2, text="Command Type: ")
    lbl1.grid(row=2, column=0, sticky=W)

    initGUI.combo = Combobox(tab2)
    initGUI.combo["values"]= ("State", "RSSI Human to Zombie", "RSSI Zombie to Human")
    initGUI.combo.current(0)
    initGUI.combo.grid(row=2, column=1)

    lbl2 = Label(tab2, text="State: ")
    lbl2.grid(row=3, column=0, sticky=W)

    initGUI.stateEntry = Entry(tab2, width=23)
    initGUI.stateEntry.grid(row=3, column=1)

    lbl3 = Label(tab2, text="RSSI: ")
    lbl3.grid(row=4, column=0, sticky=W)

    initGUI.RSSIEntry = Entry(tab2, width=23)
    initGUI.RSSIEntry.grid(row=4, column=1)

    commandButton = Button(tab2, text="Send Command", command=commandButtonCallback)
    #commandButton.pack(side = LEFT, expand = True)
    commandButton.grid(row=5, column=0)

    idleButtonTab1 = Button(tab1, text = "Idle", command = idleButtonCallback)
    #idleButtonTab1.pack(side = LEFT, expand = True)
    idleButtonTab1.grid(row=6, column=0, sticky=W)

    idleButtonTab2 = Button(tab2, text = "Idle", command = idleButtonCallback)
    #idleButtonTab2.pack(side = LEFT, expand = True)
    idleButtonTab2.grid(row=7, column=0, sticky=W)

window = Tk()
window.geometry("500x500")
window.title("Human Zombie Tag")

window.rowconfigure(0, pad=4)
window.grid_rowconfigure(2, minsize=20, weight=1)

tab_control = Notebook(window)

initGUI()

#ser = serial.Serial("COM1", 115200)

window.mainloop()
