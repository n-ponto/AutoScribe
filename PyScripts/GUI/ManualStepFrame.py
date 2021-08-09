# https://www.pythonforthelab.com/blog/handling-and-sharing-data-between-threads/
from SerialPort import SerialPort
import tkinter as tk
import time
from threading import Thread

directions =  {
    "Up":         0b1000,
    "Down":       0b0100,
    "Left":       0b0010,
    "Right":      0b0001
    }

curDirection: int = 0

serial: SerialPort
running: bool = False

timeoutSeconds = 10

def writeDirection():
    print("Starting")
    # Send the command code for manual stepping
    serial.writeByte(5)
    time.sleep(0.5)
    serial.read()
    lastActive = time.time()
    global running
    while(True):
        if (running):
            if curDirection:
                pass
                # print(curDirection)
                serial.writeByte(curDirection)
            elif (time.time()-lastActive > timeoutSeconds):
                running = True
                break
        else:
            break
    
    print("Ending")
    serial.writeByte(0b0000)
    time.sleep(0.1)
    serial.read()

def handle_keypress(event):
        print(event)
        sym = event.keysym
        direction = directions[sym]
        global curDirection
        if sym in directions and not curDirection & direction: # any([(curDirection & direction) for x in directions.values()]):
            curDirection = curDirection | direction
            print (curDirection)

class ManualStepFrame(tk.Frame):

    serial: SerialPort
    thread: Thread

    def __init__(self, master: tk.Misc, sp: SerialPort):
        super().__init__(master=master, width=200, height=200)
        self.bind("<KeyPress>", self.handle_keypress)
        self.bind("<KeyRelease>", self.handle_keyrelease)
        self.bind("<Button-1>", self.handle_click)
        self.bind("<FocusOut>", self.handle_foucus_out)
        # lbl_header = tk.Label(master=self, text="Manual Step")
        # lbl_header.pack()
        global running, serial
        serial = sp
    

    def getFrame(self, master: tk.Misc) -> tk.Frame:
        print("Getting frame")
        
        return self.frame
    
    def handle_keypress(self, event):
        print(event)
        sym = event.keysym
        direction = directions[sym]
        global curDirection
        if sym in directions and not curDirection & direction:
            curDirection = curDirection | direction
            print (curDirection)

    def handle_keyrelease(self, event):
        sym = event.keysym
        if sym in directions:
            global curDirection
            curDirection = curDirection & ~directions[sym]

    def handle_click(self, event):
        self.focus_set()
        self.config(bg="red")
        print(event)
        global running
        if not running:
            running = True
            self.thread = Thread(target=writeDirection)
            self.thread.start()
    
    def handle_foucus_out(self, event=None):
        print("Focus lost")
        self.config(bg='white')
        global running; running = False

if __name__ == "__main__":
    root = tk.Tk()
    root.title("Settings Menu")
    sp = SerialPort()
    manualStepFrame = ManualStepFrame(root, sp)
    manualStepFrame.focus_set()
    manualStepFrame.pack()
    sp.awaitResponse()
    def exit(): manualStepFrame.handle_foucus_out(); root.quit()
    root.protocol("WM_DELETE_WINDOW", exit)
    root.mainloop()
    