from Tools import SerialPort, BluetoothPort
from Frames import ManualControlFrame, SettingsFrame, DrawingFrame
from Frames.Data import DataObject
import tkinter as tk
from tkinter import ttk

dataObject: DataObject = DataObject.tryLoadData()
root = tk.Tk()
root.geometry("500x500")
root.resizable(0, 0)  # Don't resize x or y
root.title("AutoScribe GUI")
tabControl = ttk.Notebook(root)

serial = BluetoothPort()
# serial = SerialPort()

tab1 = SettingsFrame(tabControl, serial, dataObject)
tab2 = ManualControlFrame(tabControl, serial)
tab3 = DrawingFrame(tabControl, serial, dataObject)

tabControl.add(tab1, text='Settings')
tabControl.add(tab2, text='Manual Control')
tabControl.add(tab3, text='Drawing')
tabControl.pack(expand=1, fill="both")

serial.handshake(dataObject)

root.mainloop()

print("Saving settings...", end=" ")
dataObject.saveData()
print("Done.")
