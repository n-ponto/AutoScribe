from PyScripts.GUI.ManualStepFrame import ManualStepFrame
import tkinter as tk
from Settings import Settings

root = tk.Tk()
settingsMenu:tk.Frame = Settings(root)
settingsMenu.grid(row=0, column=0)
root.mainloop()

