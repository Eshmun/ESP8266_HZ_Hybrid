from tkinter import *
from tkinter import ttk


def startButtonCallback():
    print("Start")


def commandButtonCallback():
    print("Send Command")


window = Tk()
window.geometry("500x500")
window.title("Human Zombie Tag")

bottom_frame = Frame(window)
bottom_frame.pack()

top_frame = Frame(window)
top_frame.pack()


tab_control = ttk.Notebook(window)
tab1 = ttk.Frame(tab_control)
tab_control.add(tab1, text='Game Control')
tab_control.pack(expand = 1, fill = BOTH)

startButton = Button(bottom_frame, text="Start", command=startButtonCallback, font=("Arial Bold", 20))
startButton.pack()

commandButton = Button(bottom_frame, text="Send Command", command=commandButtonCallback, font=("Arial Bold", 20))
commandButton.pack()

window.mainloop()
