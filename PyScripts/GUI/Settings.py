import tkinter as tk
from Config import Default, Commands
from SerialPort import SerialPort


speed = Default.Speed
penUp = Default.UpAngle
penDown = Default.DownAngle

def Settings(master: tk.Misc, sp: SerialPort) -> tk.Frame:
    frame = tk.Frame(
        master=master,
    )

    def handleUpdate():
        u = int(ent_up.get())
        d = int(ent_down.get())
        s = int(ent_speed.get())
        if u != penUp or d != penDown:
            sp.writeByte(Commands.SET_PEN_RANGE)
            sp.writeByte(u)
            sp.writeByte(d)

    n: int = 0

    lbl_header = tk.Label(frame, text="Settings", font=24)
    lbl_header.grid(row=n, columnspan=2, pady=5)

    n+=1
    lbl_speed = tk.Label(master=frame, text="Stepper Speed")
    lbl_speed.grid(row=n, columnspan=2, pady=(5,0))
    n += 1
    lbl_speed = tk.Label(master=frame, text="Speed: ")
    ent_speed =  tk.Entry(master=frame, width=5, textvariable=tk.StringVar(frame, speed))
    lbl_speed.grid(row = n, column = 0)
    ent_speed.grid(row = n, column = 1)
    
    n+=1
    lbl_penangle = tk.Label(master=frame, text="Pen Angle")
    lbl_penangle.grid(row=n, columnspan=2, sticky="ew", pady=(5, 0))

    n+=1
    lbl_up = tk.Label(master=frame, text="Up angle: ")
    ent_up =  tk.Entry(master=frame, width=5, textvariable=tk.StringVar(frame, penUp))
    lbl_up.grid(row=n, column=0)
    ent_up.grid(row=n, column=1)
    n+=1
    lbl_down = tk.Label(master=frame, text="Down angle: ")
    ent_down =  tk.Entry(master=frame, width=5, textvariable=tk.StringVar(frame, penDown))
    lbl_down.grid(row=n, column=0)
    ent_down.grid(row=n, column=1)
    n+=1
    

    n+=1
    btn_update = tk.Button(master=frame, text="Update", command=handleUpdate)
    btn_update.grid(row=n, columnspan=2, pady=5)

    return frame


if __name__ == "__main__":
    root = tk.Tk()
    root.title("Settings Menu")
    sp = SerialPort()
    settingsMenu = Settings(root, sp)
    # sp.awaitResponse()
    settingsMenu.pack()
    root.mainloop()