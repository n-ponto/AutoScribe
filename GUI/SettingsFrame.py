import time
import sys
import os
import tkinter as tk
from Data import DataObject
# Import Serial Port and Encodings
sys.path.append(os.path.dirname(__file__) + "/../..")
from Tools.SerialPort import SerialPort
from Tools.Encodings import *

MIN_ANGLE = 20
MAX_ANGLE = 140


class PenAngleEntry(tk.Entry):

    _serial: SerialPort = None

    def __init__(self, master: tk.Misc, sp: SerialPort, initVal: int):
        sv = tk.IntVar()
        super().__init__(master=master, width=5, textvariable=tk.IntVar(master, initVal))
        self.bind("<Up>", self._up)
        self.bind("<Down>", self._down)
        self.bind("<Return>", self._modified)
        self._serial = sp

    def _up(self, event):
        print("up")
        value = int(self.get()) + 1
        self.delete(0, tk.END)
        self.insert(0, value)
        self._modified()

    def _down(self, event):
        value = int(self.get()) - 1
        self.delete(0, tk.END)
        self.insert(0, value)
        self._modified()

    def _modified(self, event=None):
        print("modified")
        value = int(self.get())
        if (MIN_ANGLE <= value and value <= MAX_ANGLE):
            self._serial.writeByte(value)
            time.sleep(0.1)
            self._serial.readStr()
            return
        print(f"[ERROR] invalid value {self.get()}")


class TunePenFrame(tk.Frame):

    _serial: SerialPort = None
    _ent_up: PenAngleEntry
    _ent_down: PenAngleEntry

    def __init__(self, master: tk.Misc, sp: SerialPort, dataObject: DataObject):
        super().__init__(master=master)
        self.bind("<FocusIn>", self._focusIn)
        self.bind("<FocusOut>", self._focusOut)
        self.bind("<Return>", self._saveAngles)  # Save when enter pressed
        self._serial = sp
        self._dataObject = dataObject
        self._render()

    def _render(self):
        title = tk.Label(master=self, text="Tune Pen", font='Helvetica 12 bold')
        title.grid(row=0, columnspan=2, pady=5)

        lbl_up = tk.Label(master=self, text="Up height: ")
        self._ent_up = PenAngleEntry(self, self._serial, self._dataObject.PenUpHeight)
        lbl_up.grid(row=1, column=0)
        self._ent_up.grid(row=1, column=1)

        lbl_down = tk.Label(master=self, text="Down height: ")
        self._ent_down = PenAngleEntry(self, self._serial, self._dataObject.PenDownHeight)
        lbl_down.grid(row=2, column=0)
        self._ent_down.grid(row=2, column=1)
        # Update button
        btn_update = tk.Button(
            master=self, text="Save angles", command=self._saveAngles)
        btn_update.grid(row=3, columnspan=2, pady=5)

    def _saveAngles(self):
        # Leave change pen angle command
        self._serial.writeByte(0xFF)
        time.sleep(0.1)
        self._serial.readStr()
        # Set pen range
        self._serial.writeByte(Commands.SET_PEN_RANGE)
        self._serial.writeByte(int(self._ent_up.get()))
        self._serial.writeByte(int(self._ent_down.get()))
        time.sleep(0.1)
        self._serial.readStr()
        # Re-enter change pen angle
        self._serial.writeByte(Commands.CHANGE_PEN_ANGLE)
        time.sleep(0.1)
        self._serial.readStr()
        # Save the new pen heights
        self._dataObject.PenUpHeight = int(self._ent_up.get())
        self._dataObject.PenDownHeight = int(self._ent_down.get())

    def _focusIn(self, event):
        '''
        Start the manual control runtime mode
        '''
        print("Focus in on settings frame")
        self._serial.writeByte(Commands.CHANGE_PEN_ANGLE)
        time.sleep(0.1)
        self._serial.readStr()

    def _focusOut(self, event=None):
        '''
        End the manual control runtime mode
        '''
        print("Focus out on settings frame")
        self._serial.writeByte(0xFF)
        time.sleep(0.1)
        self._serial.readStr()

# class StepperDelayFrame(tk.Frame):
#     DEFAULT_DELAY =  

#     def __init__(self, master: tk.Misc, sp: SerialPort):
#         super().__init__(master=master)
#         self._serial = sp
#         self._render()

#     def _render(self):
#         title = tk.Label(master=self, text="Tune Pen", font='Helvetica 12 bold')
#         title.pack(pady=5)

#         entry = tk.Entry(master=self, width=5, textvariable=)

#         btn_update = tk.Button(
#             master=self, text="Update delay", command=self._saveAngles)
#         btn_update.pack(pady=5)

class SettingsFrame(tk.Frame):
    def __init__(self, master: tk.Misc, sp: SerialPort, dataObject: DataObject):
        super().__init__(master=master)
        self._serial = sp
        self._dataObject = dataObject
        self._render()
    
    def _render(self):
        tune_pen_frame = TunePenFrame(self, self._serial, self._dataObject )
        tune_pen_frame.grid(row=0, column=0)



################################################################################

if __name__ == "__main__":
    root = tk.Tk()  # Create root window
    root.title("Settings Frame")  # Set window title
    sp = SerialPort()  # Initialize serial port
    SettingsFrame = SettingsFrame(root, sp)  # Create frame
    SettingsFrame.pack()  # Add the frame to the root window
    SettingsFrame.focus_set()
    sp.awaitResponse()

    root.mainloop()
    sp.readStr()
