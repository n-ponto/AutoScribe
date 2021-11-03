from tkinter.constants import LEFT, TRUE
from SerialPort import SerialPort
import tkinter as tk
from time import sleep

PEN_UP =   0b10000<<11
PEN_DOWN = 0b01000<<11
END =      0b11111<<11
DRAW_COMMAND = 4

class DrawingFrame(tk.Frame):

    serial: SerialPort
    # tempLine

    def __init__(self, master: tk.Misc, serial: SerialPort):
        super().__init__(master=master)
        self.serial = serial
        self.x = self.y = None
        self.tempLine = None
        self.drawWidgets()

    def handle_click(self, event):
        event.widget.focus_set()
        if self.tempLine is None:
            x0 = event.x
            y0 = event.y
        else:
            coords = event.widget.coords(self.tempLine)
            x0 = coords[2]
            y0 = coords[3]
            print("Old end coordinates: ", x0, y0)
            event.widget.itemconfig(self.tempLine, fill='blue')
        self.tempLine = event.widget.create_line(x0, y0, event.x, event.y, width=5, fill="red")
        print("Created new line", self.tempLine)
        
    def handle_doubleclick(self, event):
        self.tempLine = None

    def handle_motion(self, event):
        if self.tempLine:
            try:
                coords = event.widget.coords(self.tempLine)
                coords[2] = event.x
                coords[3] = event.y
                event.widget.coords(self.tempLine, *coords)
            except Exception:
                print('ERROR')
                print('self.tempLine', self.tempLine)
                print('self.canvas.find_all()', self.canvas.find_all())
            
    def send(self):
        print('Extracting Instructions...')
        points = []
        lines = self.canvas.find_all()
        prevEnd = None
        # Extract Instructions
        for line in lines:
            coords = self.canvas.coords(line)
            print(coords)
            start = tuple(coords[:2])
            end = tuple(coords[2:])
            penDown:bool = False
            if (start != prevEnd):
                sx, sy = start
                start = (PEN_UP + sx, sy)
                points.append(start)
                penDown = True
            prevEnd = end
            if (end != start):
                if penDown: end = (end[0] + PEN_DOWN, end[1])
                points.append(end)
        # Send instructions
        print('Sending instructions...')
        self.serial.writeByte(DRAW_COMMAND)
        sleep(0.1)
        self.serial.read()
        self.canvas.update()
        print("Mapped points: ")
        for x, y in points:
            # Map to same coordinate as machine
            y = self.canvas.winfo_height() - y
            # Print
            if (int(x)>>11 == PEN_UP>>11):
                print("UP", x-PEN_UP, y)
            elif (int(x)>>11 == PEN_DOWN>>11):
                print("DOWN", x-PEN_DOWN, y)
            else:
                print(x, y)
            self.serial.writePoint(int(x), int(y))
            self.serial.read()
            sleep(0.2)

        # Return home when done
        self.serial.writePoint(PEN_UP, 0)

        self.serial.writePoint(END, 0)
        print('Done sending instructions.')
        sleep(0.1)
        self.serial.read()
        


    def undo(self, event):
        print("undo")
        if self.tempLine:
            self.canvas.delete(self.tempLine)
            self.tempLine = self.tempLine-1
            self.canvas.itemconfig(self.tempLine, fill='red')
            if len(self.canvas.find_all()) == 0:
                self.tempLine = None
        elif len(self.canvas.find_all()) > 0:
            last = self.canvas.find_all()[-1]
            print(self.canvas.find_all())
            print(last)
            self.canvas.delete(last)
        else:
            print("Nothing to undo.")

    def clear(self, event=None):
        self.tempLine = None
        self.canvas.delete(tk.ALL)

    def drawWidgets(self):
        self.canvas = tk.Canvas(self, width=500, height=500)
        self.canvas.bind('<Button-1>', self.handle_click)
        self.canvas.bind("<Motion>", self.handle_motion)
        self.canvas.bind('<Double-Button-1>', self.handle_doubleclick)
        self.canvas.bind('<Escape>', self.clear)
        self.canvas.bind('<Control-z>', self.undo)
        self.canvas.pack(fill=tk.BOTH, expand=True)

        # Buttons
        frm_bttns = tk.Frame(self)
        btn_clear = tk.Button(master=frm_bttns, text="Clear", command=self.clear)
        btn_send = tk.Button(master=frm_bttns, text="Send", command=self.send)
        btn_clear.pack(side=tk.LEFT)
        btn_send.pack(side=tk.RIGHT)
        frm_bttns.pack(fill=tk.X)
        


if __name__ == '__main__':
    root = tk.Tk()
    root.title('DrawingFrame')
    serial = SerialPort()
    serial.awaitResponse()
    df = DrawingFrame(root, serial)
    df.pack(fill=tk.BOTH, expand=True)
    root.mainloop()
