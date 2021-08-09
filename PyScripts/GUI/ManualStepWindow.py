import os, sys
sys.path.append(os.path.dirname(__file__ ) + "/..")
from Tools.SerialPort import SerialPort
import tkinter as tk
import threading
import time

curDirection: int = 0

directions =  {
    "Up":         0b1000,
    "Down":       0b0100,
    "Left":       0b0010,
    "Right":      0b0001
    }

class Window(threading.Thread):

    def __init__(self):
        threading.Thread.__init__(self)
        self.running: bool = True
        self.start()

    def callback(self):
        self.running = False
        self.root.quit()
    
    def run(self):
        self.root = tk.Tk()

        def handle_keypress(event):
            sym = event.keysym
            direction = directions[sym]
            global curDirection 
            if sym in directions and not curDirection & direction: # any([(curDirection & direction) for x in directions.values()]):
                curDirection = curDirection | direction
                # print (curDirection)

        def handle_keyrelease(event):
            sym = event.keysym
            if sym in directions:
                global curDirection
                curDirection = curDirection & ~directions[sym]

        self.root.bind("<KeyPress>", handle_keypress)
        self.root.bind("<KeyRelease>", handle_keyrelease)

        self.root.protocol("WM_DELETE_WINDOW", self.callback)
        self.root.mainloop()

def on_opening():
    pass

def on_closing():
    print("Ending")
    # Send a null byte to end manual stepping
    serialPort.writeByte(0b0000)
    window.destroy()

                # serialPort.writeByte(curDirection)

window = Window()
serialPort = SerialPort()
serialPort.awaitResponse()


print("Starting")
# Send the command code for manual stepping
serialPort.writeByte(5)
time.sleep(0.5)
serialPort.read()
privateDir: int
while(True):
    if (window.running):
        if curDirection:
            # print(curDirection)
            serialPort.writeByte(curDirection)
    else:
        break

print("Ending")
serialPort.writeByte(0b0000)
time.sleep(0.1)
serialPort.read()


