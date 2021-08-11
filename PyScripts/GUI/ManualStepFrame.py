# https://www.pythonforthelab.com/blog/handling-and-sharing-data-between-threads/
from SerialPort import SerialPort
import tkinter as tk
import time
from threading import Thread

directions =  {
    "up":         0b1000,
    "down":       0b0100,
    "left":       0b0010,
    "right":      0b0001,
    "w":         0b1000,
    "s":       0b0100,
    "a":       0b0010,
    "d":      0b0001
    }

PEN_UP = 0x10
PEN_DOWN = 0x11

TIMEOUT_SECONDS = 10
MANUAL_STEP_COMMAND = 5

curDirection: int = 0

serial: SerialPort
running_flag: bool = False
pen_flag: bool = True
penIsUp: bool = True

# Function designed to be run in it's own thread so it can send continuous instructions
# to the arduino without handling user input directly
def writeDirection():
    print("Starting")
    serial.writeByte(MANUAL_STEP_COMMAND) # Send the command code for manual stepping
    serial.writeByte(PEN_UP) # Send code to initilize the pen in the up position
    time.sleep(0.5)
    serial.read()
    lastActive = time.time()
    global running_flag, pen_flag, penIsUp
    while(True):
        if (running_flag):
            global pen_flag
            if pen_flag:
                if penIsUp:
                    penIsUp = False
                    serial.writeByte(PEN_DOWN)
                else:
                    penIsUp = True
                    serial.writeByte(PEN_UP)
                pen_flag = False
            elif curDirection:
                serial.writeByte(curDirection)
            elif (time.time()-lastActive > TIMEOUT_SECONDS):
                print("Timeout")
                running_flag = False
                break
            lastActive = time.time()
        else:
            break
    serial.writeByte(0b0000) # Null byte exits from manual step mode
    time.sleep(0.1)
    serial.read()
    print("Manual Step Thread Finished") 

# Should be called on exit to ensure the thread is finished
def exit():
    global running_flag
    if running_flag:
        print("Ending Manual Step Thread...")
        running_flag = False

# Creates the manual step frame
class ManualStepFrame(tk.Frame):

    thread: Thread

    def __init__(self, master: tk.Misc, sp: SerialPort, exit_functions: list):
        super().__init__(master=master, width=200, height=200)
        self.bind("<KeyPress>", self.handle_keypress)
        self.bind("<KeyRelease>", self.handle_keyrelease)
        self.bind("<Button-1>", self.handle_click)
        self.bind("<FocusOut>", self.handle_foucus_out)
        exit_functions.append(exit)
        global running_flag, serial
        serial = sp
    
    def handle_keypress(self, event):
        sym: str = event.keysym.lower()
        global curDirection, pen_flag
        if sym in directions and not curDirection & directions[sym]:
            curDirection = curDirection | directions[sym]
            print (curDirection)
        elif sym == 'space' and penIsUp:
            print("Pen Down")
            pen_flag = True        

    def handle_keyrelease(self, event):
        sym = event.keysym.lower()
        global pen_flag, curDirection
        if sym in directions:
            curDirection = curDirection & ~directions[sym]
        elif sym == 'space' and not penIsUp:
            print("Pen Up")
            pen_flag = True

    def handle_click(self, event):
        self.focus_set()
        self.config(bg="red")
        print(event)
        global running_flag
        if not running_flag:
            running_flag = True
            self.thread = Thread(target=writeDirection)
            self.thread.start()
    
    def handle_foucus_out(self, event=None):
        print("Focus lost")
        self.config(bg='white')
        global running_flag; running_flag = False



################################################################################


if __name__ == "__main__":
    root = tk.Tk()
    root.title("Settings Menu")
    sp = SerialPort()
    manualStepFrame = ManualStepFrame(root, sp, list())
    manualStepFrame.focus_set()
    manualStepFrame.pack()
    sp.awaitResponse()
    def dev_exit(): exit(); root.quit()
    root.protocol("WM_DELETE_WINDOW", dev_exit)
    root.mainloop()
    