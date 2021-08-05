import tkinter as tk

# Creating a window
window = tk.Tk()

# Labels
greeting = tk.Label(text="hello")
greeting.pack()

label = tk.Label(
    text="Hellow, Tkinter",
    foreground="white",
    background="black"
)
label.pack()

labelColor = tk.Label(text="Hello", background="#34A2FE")
labelColor.pack()

labelShort = tk.Label(text="Hello", fg="white", bg="black")
labelShort.pack()

labelHeight = tk.Label(
    text="Hello",
    fg="white",
    bg="black",
    width=10,
    height=10
    )
labelHeight.pack()

#Buttons
button = tk.Button(
    text="Click me!",
    width=25,
    height=5,
    bg="blue",
    fg="yellow",
)
button.pack()

#Entry Widget
entry = tk.Entry(fg="yellow", bg="blue", width=50)
entry.pack()

name = entry.get()
print(name)

window.mainloop()
