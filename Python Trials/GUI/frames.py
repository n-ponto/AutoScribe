import tkinter as tk
from typing import Collection

def borderTypes():
    border_effects = {
        "flat": tk.FLAT,
        "sunken": tk.SUNKEN,
        "raised": tk.RAISED,
        "groove": tk.GROOVE,
        "ridge": tk.RIDGE,
    }
    window = tk.Tk()

    for relief_name, relief in border_effects.items():
        frame = tk.Frame(master=window, relief=relief, borderwidth=5)
        frame.pack(side=tk.LEFT)
        label = tk.Label(master=frame, text=relief_name)
        label.pack()

    window.mainloop()

def fillingWindow():
    import tkinter as tk

    window = tk.Tk()

    frame1 = tk.Frame(master=window, height=100, bg="red")
    frame1.pack(fill=tk.X)

    frame2 = tk.Frame(master=window, height=50, bg="yellow")
    frame2.pack(fill=tk.X)

    frame3 = tk.Frame(master=window, height=25, bg="blue")
    frame3.pack(fill=tk.X)

    window.mainloop()

def packLeft():
    window = tk.Tk()

    frame1 = tk.Frame(master=window, width=200, height=100, bg="red")
    frame1.pack(fill=tk.Y, side=tk.LEFT)

    frame2 = tk.Frame(master=window, width=100, bg="yellow")
    frame2.pack(fill=tk.Y, side=tk.LEFT)

    frame3 = tk.Frame(master=window, width=50, bg="blue")
    frame3.pack(fill=tk.Y, side=tk.LEFT)

    window.mainloop()

def expand():
    window = tk.Tk()

    frame1 = tk.Frame(master=window, width=200, height=100, bg="red")
    frame1.pack(fill=tk.BOTH, side=tk.LEFT, expand=True)

    frame2 = tk.Frame(master=window, width=100, bg="yellow")
    frame2.pack(fill=tk.BOTH, side=tk.LEFT, expand=True)

    frame3 = tk.Frame(master=window, width=50, bg="blue")
    frame3.pack(fill=tk.BOTH, side=tk.LEFT, expand=True)

    window.mainloop()

def place():
    window = tk.Tk()
    frame = tk.Frame(master=window, width=150, height=150)
    frame.pack()
    label1 = tk.Label(master=frame, text="I'm at (0, 0)", bg="red")
    label1.place(x=0, y=0)
    label2 = tk.Label(master=frame, text="I'm at (75, 75)", bg="yellow")
    label2.place(x=75, y=75)
    window.mainloop()

def resizingGrid():
    window = tk.Tk()

    for i in range(3):
        window.columnconfigure(i, weight=1, minsize=75)
        window.rowconfigure(i, weight=1, minsize=50)

        for j in range(0, 3):
            frame = tk.Frame(
                master=window,
                relief=tk.RAISED,
                borderwidth=1
            )
            frame.grid(row=i, column=j, padx=5, pady=5)

            label = tk.Label(master=frame, text=f"Row {i}\nColumn {j}")
            label.pack(padx=5, pady=5)

    window.mainloop()


def directions():
    window = tk.Tk()
    window.columnconfigure(0, minsize=250)
    window.rowconfigure(list(range(2)), minsize=100)

    label1 = tk.Label(text="A", bg="yellow")
    label1.grid(row=0, column=0, sticky="ne")

    label2 = tk.Label(text="B", bg="blue")
    label2.grid(row=1, column=0, sticky="sw")

    window.mainloop()

def fillingGridCells():
    window = tk.Tk()
    window.rowconfigure(0, minsize=50)
    window.columnconfigure([0, 1, 2], weight=1, minsize=50)
    window.columnconfigure(3, weight=1, minsize=50)

    label1 = tk.Label(text="1", bg="black", fg="white")
    label2 = tk.Label(text="2", bg="black", fg="white")
    label3 = tk.Label(text="3", bg="black", fg="white")
    label4 = tk.Label(text="4", bg="black", fg="white")

    label1.grid(row=0, column=0)
    label2.grid(row=0, column=1, sticky="ew")
    label3.grid(row=0, column=2, sticky="ns")
    label4.grid(row=0, column=3, sticky="nsew")

    window.mainloop()

def binding():
    window = tk.Tk()
    def handle_keypress(event):
        """Print the character associated to the key pressed"""
        print(event.char)
    # Bind keypress event to handle_keypress()
    window.bind("<Key>", handle_keypress)
    window.mainloop()

def clicking():
    window = tk.Tk()
    def handle_click(event):
        print("The button was clicked!")

    button = tk.Button(text="Click me!")
    button.pack()
    button.bind("<Button-1>", handle_click)
    window.mainloop()

def usingCommand():
    window = tk.Tk()

    def inc():
        number = int(lbl_value["text"])
        lbl_value["text"] = f"{number + 1}"

    def dec():
        number = int(lbl_value["text"])
        lbl_value["text"] = f"{number-1}"

    window.rowconfigure(0, minsize=50, weight=1)
    window.columnconfigure([0, 1, 2], minsize=50, weight=1)

    btn_decrease = tk.Button(master=window, text="-", command=dec)
    btn_decrease.grid(row=0, column=0, sticky="nsew")
    # btn_decrease.bind("<Button-1>", dec)

    lbl_value = tk.Label(master=window, text="0")
    lbl_value.grid(row=0, column=1)

    btn_increase = tk.Button(master=window, text="+", command=inc)
    btn_increase.grid(row=0, column=2, sticky="nsew")
    # btn_increase.bind("<Button-1>", inc)

    window.mainloop()

def temperatureConverter():

    def fToC(event=None):
        f = ent_temperature.get()
        celsius = (5/9) * (float(f) - 32)
        lbl_result["text"] = f"{round(celsius, 2)} \N{DEGREE CELSIUS}"

    window = tk.Tk()
    window.title("Temperature Converter")
    frm_entry = tk.Frame(master=window)
    ent_temperature = tk.Entry(master=frm_entry, width=10)
    lbl_temp = tk.Label(master=frm_entry, text="\N{DEGREE FAHRENHEIT}")

    ent_temperature.grid(row=0, column=0, sticky="e")
    lbl_temp.grid(row=0, column=1, sticky="w" )

    btn_convert = tk.Button(
        master=window,
        text= "\N{RIGHTWARDS BLACK ARROW}",
        command=fToC
    )
    lbl_result = tk.Label(master=window, text="\N{DEGREE CELSIUS}")
    frm_entry.grid(row=0, column=0, padx=10)
    btn_convert.grid(row=0, column=1, pady=10)
    lbl_result.grid(row=0, column=2, padx=10)

    window.mainloop()

temperatureConverter()
