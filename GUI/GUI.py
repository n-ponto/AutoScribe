import sys, os

from serial.serialwin32 import Serial
sys.path.append(os.path.dirname(__file__) + "/..")
from Tools.SerialPort import SerialPort
# Import frames
from Frames.ManualControlFrame import ManualControlFrame
from Frames.SettingsFrame import SettingsFrame
from Frames.DrawingFrame import DrawingFrame
# Import tkinter libraries
import tkinter as tk
from tkinter import ttk

root = tk.Tk()
root.geometry("500x500")
root.resizable(0, 0) # Don't resize x or y
root.title("AutoScribe GUI")
tabControl = ttk.Notebook(root)

serial = SerialPort()

tab1 = SettingsFrame(tabControl, serial)
tab2 = ManualControlFrame(tabControl, serial)
tab3 = DrawingFrame(tabControl, serial)
  
tabControl.add(tab1, text ='Settings')
tabControl.add(tab2, text ='Manual Control')
tabControl.add(tab3, text ='Drawing')
tabControl.pack(expand = 1, fill ="both")

serial.awaitResponse()
root.mainloop()  



 