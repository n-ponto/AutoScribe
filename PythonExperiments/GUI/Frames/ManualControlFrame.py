import time
import sys
import os
import tkinter as tk
from tkinter.constants import DISABLED
# Import Serial Port and Encodings
sys.path.append(os.path.dirname(__file__) + "/../..")
from Tools.Encodings import *
from Tools.SerialPort import SerialPort

# Macros for accessing the directions dictionary
PRESS = 0
RELEASE = 1

# Mapping of keys to the corresponding pressed/released signal
directions = {
    "up":    (MCKeys.UP_P, MCKeys.UP_R),
    "down":  (MCKeys.DN_P, MCKeys.DN_R),
    "left":  (MCKeys.LT_P, MCKeys.LT_R),
    "right": (MCKeys.RT_P, MCKeys.RT_R),
    "w": (MCKeys.UP_P, MCKeys.UP_R),
    "s": (MCKeys.DN_P, MCKeys.DN_R),
    "a": (MCKeys.LT_P, MCKeys.LT_R),
    "d": (MCKeys.RT_P, MCKeys.RT_R),
    "space": (MCKeys.SP_P, MCKeys.SP_R)
}


class ManualControlFrame(tk.Frame):

    _serial: SerialPort = None
    _focused: bool = False
    _btn_start: tk.Button
    _btn_end: tk.Button

    def __init__(self, master: tk.Misc, sp: SerialPort):
        '''
        Initialize the Manual Step frame with keybindings and serial port
        '''
        super().__init__(master=master)
        self._serial = sp
        self.bind("<FocusOut>", self._focusOut)
        self._render()

    def _focusOut(self, event):
        print("focus out of manual control frame")
        self._end()

    def _render(self):
        padding: int = 5
        self._btn_start = tk.Button(
            master=self, text="Start manual control", command=self._start)
        self._btn_start.pack(padx = padding, pady = padding)
        self._btn_end = tk.Button(
            master=self, text="End manual control", command=self._end, state='disabled')
        self._btn_end.pack(padx = padding, pady = padding) 

    def _start(self):
        # Track key presses
        print("Start")
        self._btn_start['state'] = 'disabled'
        self.focus_set()
        self.bind("<KeyPress>", self._keypress)
        self.bind("<KeyRelease>", self._keyrelease)
        print("Starting manual control")
        self._serial.writeByte(Commands.ENTER_MANUAL_CONTROL_MODE)
        time.sleep(0.1)  # Wait for manual control mode to start
        self._serial.read()
        self._btn_end['state'] = 'normal'

    def _end(self):
        if (self._btn_end['state'].lower() == 'disabled'):
            return
        # Stop tracking key presses
        self._btn_end['state'] = 'disabled'
        self.unbind("<KeyPress>")
        self.unbind("<KeyRelease>")
        self._serial.writeByte(MCKeys.STOP)  # Send stop signal
        time.sleep(0.1)  # Wait for manual control mode to stop
        self._serial.read()  # Read from serial
        self._btn_start['state'] = 'normal'

    def _keypress(self, event):
        '''
        Send keypress signal to arduino
        '''
        sym: str = event.keysym.lower()
        if sym in directions:
            self._serial.writeByte(directions[sym][PRESS])
        else:
            print(f"Unhandled key press: {sym}")

    def _keyrelease(self, event):
        '''
        Send key release signal to arduino
        '''
        sym: str = event.keysym.lower()
        if sym in directions:
            self._serial.writeByte(directions[sym][RELEASE])
        else:
            print(f"Unhandled key release: {sym}")

################################################################################

if __name__ == "__main__":
    root = tk.Tk()  # Create root window
    root.title("Manual Control Frame")  # Set window title
    root.geometry('500x500')
    sp = SerialPort()  # Initialize serial port
    manualStepFrame = ManualControlFrame(root, sp)  # Create frame
    manualStepFrame.pack()  # Add the frame to the root window
    sp.awaitResponse()

    root.mainloop()
    sp.read()
