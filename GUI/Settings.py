import tkinter as tk
from time import sleep
import sys, os
sys.path.append(os.path.dirname(__file__) + "/..")
from Tools.Encodings import *
from Tools.SerialPort import SerialPort

# Globals used to keep track of the last value sent to the arduino
speed = 2000
penUp = 50
penDown = 70

def Settings(master: tk.Misc, serial: SerialPort) -> tk.Frame:
    frame = tk.Frame(
        master=master,
    )

    # Header
    n: int = 0
    lbl_header = tk.Label(frame, text="Settings", font=24)
    lbl_header.grid(row=n, columnspan=2, pady=5)

    # Stepper Speed
    n+=1
    lbl_speed = tk.Label(master=frame, text="Stepper Speed")
    lbl_speed.grid(row=n, columnspan=2, pady=(5,0))
    n += 1
    lbl_speed = tk.Label(master=frame, text="Speed: ")
    ent_speed =  tk.Entry(master=frame, width=5, textvariable=tk.StringVar(frame, speed))
    lbl_speed.grid(row = n, column = 0)
    ent_speed.grid(row = n, column = 1)
    
    # Pen Angle
    n+=1
    lbl_penangle = tk.Label(master=frame, text="Pen Angle")
    lbl_penangle.grid(row=n, columnspan=2, sticky="ew", pady=(5, 0))
    n+=1
    lbl_up = tk.Label(master=frame, text="Up height: ")
    ent_up =  tk.Entry(master=frame, width=5, textvariable=tk.StringVar(frame, penUp))
    lbl_up.grid(row=n, column=0)
    ent_up.grid(row=n, column=1)
    n+=1
    lbl_down = tk.Label(master=frame, text="Down height: ")
    ent_down =  tk.Entry(master=frame, width=5, textvariable=tk.StringVar(frame, penDown))
    lbl_down.grid(row=n, column=0)
    ent_down.grid(row=n, column=1)
    n+=1
    
    # Update Function
    def handleUpdate():
        u = int(ent_up.get())
        d = int(ent_down.get())
        s = int(ent_speed.get())
        global penUp, penDown, speed
        if u != penUp or d != penDown:
            serial.writeByte(Commands.SET_PEN_RANGE)
            serial.writeByte(u)
            serial.writeByte(d)
            penUp = u
            penDown = d
        
        if s != speed:
            serial.writeByte(Commands.SET_STEPPER_DELAY)
            serial.writeShort(s)
            speed = s
        sleep(0.1)
        serial.readStr()

    # Update Button
    n+=1
    btn_update = tk.Button(master=frame, text="Update", command=handleUpdate)
    btn_update.grid(row=n, columnspan=2, pady=5)

    # Reset Home Function
    def handleResetHome():
        serial.writeByte(Commands.RESET_HOME)
        serial.readStr()

    n+=1
    btn_rsthm = tk.Button(master=frame, text="Reset Home", command=handleResetHome)
    btn_rsthm.grid(row=n, columnspan=2, pady=5)

    return frame


if __name__ == "__main__":
    root = tk.Tk()
    root.title("Settings Menu")
    serial = SerialPort()
    settingsMenu = Settings(root, serial)
    # serial.awaitResponse()
    settingsMenu.pack()
    root.mainloop()