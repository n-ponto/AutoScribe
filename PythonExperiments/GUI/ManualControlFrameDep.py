import time
import sys
import os
import tkinter as tk
# Import Serial Port and Encodings
sys.path.append(os.path.dirname(__file__) + "/..")
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


class ManualStepFrame(tk.Frame):

    _serial: SerialPort = None
    _focused: bool = False

    def __init__(self, master: tk.Misc, sp: SerialPort):
        '''
        Initialize the Manual Step frame with keybindings and serial port
        '''
        super().__init__(master=master, width=200, height=200)
        self.bind("<Button-1>", self._focusIn)
        self.bind("<FocusOut>", self._focusOut)
        self.bind("<KeyPress>", self._keypress)
        self.bind("<KeyRelease>", self._keyrelease)
        self.focus_set()  # Do we need this?
        self._serial = sp
        self.config(bg="blue")

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

    def _focusIn(self, event):
        '''
        Start the manual control runtime mode
        '''
        if self._focused:
            return
        self._focused = True
        self.config(bg="red")  # Make the background red
        print("Starting manual control")
        self._serial.writeByte(Commands.ENTER_MANUAL_CONTROL_MODE)
        time.sleep(0.1)  # Wait for manual control mode to start
        self._serial.read()

    def _focusOut(self, event=None):
        '''
        End the manual control runtime mode
        '''
        print("Focus lost")
        self._focused = False
        self.config(bg='blue')  # Set background to red
        self._serial.writeByte(MCKeys.STOP)  # Send stop signal
        time.sleep(0.1)  # Wait for manual control mode to stop
        self._serial.read()  # Read from serial

################################################################################

if __name__ == "__main__":
    root = tk.Tk()  # Create root window
    root.title("Manual Control Frame")  # Set window title
    sp = SerialPort()  # Initialize serial port
    manualStepFrame = ManualStepFrame(root, sp)  # Create frame
    # manualStepFrame.focus_set()
    manualStepFrame.pack()  # Add the frame to the root window
    sp.awaitResponse()

    root.mainloop()
    sp.read()
