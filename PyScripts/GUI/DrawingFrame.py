from tkinter.constants import LEFT, TRUE
from SerialPort import SerialPort
import tkinter as tk


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
            except Exception:
                print('ERROR')
                print('self.tempLine', self.tempLine)
                print('self.canvas.find_all()', self.canvas.find_all())
            coords[2] = event.x
            coords[3] = event.y
            event.widget.coords(self.tempLine, *coords)

    def send(self):
        print('Sending coordinates...')
        points = []
        lines = self.canvas.find_all()
        prevEnd = None
        for line in lines:
            coords = self.canvas.coords(line)
            print(coords)
            start = tuple(coords[:2])
            end = tuple(coords[2:])
            if (start != prevEnd):
                points.append("pen up")
                points.append(start)
                points.append("pen down")
            prevEnd = end
            points.append(end)
        print(points)

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
    serial = None # SerialPort()
    df = DrawingFrame(root, serial)
    df.pack(fill=tk.BOTH, expand=True)
    root.mainloop()
