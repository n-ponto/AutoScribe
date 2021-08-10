import tkinter as tk
from Settings import Settings
from SerialPort import SerialPort
from ManualStepFrame import ManualStepFrame

# Functions to run during closing

root = tk.Tk()

exit_functions: list = [root.quit]

def exitScript():
    for func in exit_functions:
        func()

root.protocol("WM_DELETE_WINDOW", exitScript)

serial = SerialPort()
settingsMenu:tk.Frame = Settings(root, serial)
settingsMenu.grid(row=0, column=0)
manualStepFrame = ManualStepFrame(root, serial, exit_functions)
manualStepFrame.grid(row=0, column=1)

serial.awaitResponse()
root.mainloop()



