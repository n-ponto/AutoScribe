# https://stackoverflow.com/questions/40877429/drawing-straight-lines-on-a-tkinter-canvas-with-grid-on-python-with-mouse
from tkinter import *

current = None

def DrawGrid(drawing_area, line_distance):

   for x in range(line_distance,600,line_distance):
       drawing_area.create_line(x, 0, x, 600, fill="#d3d3d3")

   for y in range(line_distance,600,line_distance):
       drawing_area.create_line(0, y, 600, y, fill="#d3d3d3")

def main():
    root = Tk()
    drawing_area = Canvas(root, width=600, height=600, bg='white')
    drawing_area.pack()
    DrawGrid(drawing_area, 10)
    drawing_area.bind("<Escape>", reset)
    drawing_area.bind("<Motion>", motion)
    drawing_area.bind("<ButtonPress-1>", Mousedown)

    root.mainloop()

def reset(event):
    global current
    current = None

def Mousedown(event):
    global current

    event.widget.focus_set()  # so escape key will work

    if current is None:
        # the new line starts where the user clicked
        x0 = event.x
        y0 = event.y

    else:
        # the new line starts at the end of the previously
        # drawn line
        coords = event.widget.coords(current)
        x0 = coords[2]
        y0 = coords[3]

    # create the new line
    current = event.widget.create_line(x0, y0, event.x, event.y)

def motion(event):
    if current:
        # modify the current line by changing the end coordinates
        # to be the current mouse position
        coords = event.widget.coords(current)
        coords[2] = event.x
        coords[3] = event.y

        event.widget.coords(current, *coords)

main()