import math
from PathVisualizer import PathVisualizer
import time

# Uses bresenham algorithm to make 1/8th of a circle from
# 3 o'clock to 1:30
def bresenham(r: int, v=False) -> list:
    coords = []
    if v: print(f"BRESENHAM\tr={r}")
    x = r-1
    y = 0
    P = 1 - r
    while x >= y:
        # Inside or on perimeter
        if P <= 0:
            P = P + 2 * y + 1
        else:
            x -= 1
            P = P + 2 * y - 2 * x + 1
        
        if x >= y: 
            coords.append((x, y))
            if v: print(f"Add: ({x}, {y})")
        y += 1
    return coords

# Reflects bresenham across diagonal to make 1/4 of a 
# circle from 3 to 12 o'clock
def quarter_circle(r: int, v=False) -> list:
    coords: list = bresenham(r, v)
    if v: print("QUARTER")
    finalIndex = len(coords)-1
    start = finalIndex-1 if coords[finalIndex][0] == coords[finalIndex][1] else finalIndex
    # Add x values
    reverse_indexes = range(start, -1, -1)
    for i in reverse_indexes:
        x, y = coords[i]
        newX = y
        newY = x
        coords.append( (newX, newY) )
        if v: print(f"copy: ({newX}, {newY})")

    return coords

# Translates and reflects a quarter of a circle to produce 
# a full circle starting and ending on the middle right
def right_circle(d: int, v=False) -> list:
    r = int((d+1)/2)
    center = int(d/2)
    # Get circle from 3 to 12 o'clock
    quarter: list = quarter_circle(r, v)
    coords: list = []
    oddDiameter = int(center != r)

    if v: print(f"RIGHT\tdiameter={d}\tradius={r}")
    # Map onto top right
    for x, y in quarter:
        a = x + center
        b = y + center
        coords.append( (a, b) )
        if v: print(f"map:\t  ({x}, {y}) -> ({a}, {b})")   
    if v: print("-" * 20)
    # Draw circle from 12 to 9 o'clock (mirror across x axis)
    start:int = len(quarter)-1 - oddDiameter
    for i in range(start, -1, -1):
        x, y = quarter[i]
        a = r - x - 1
        b = y + center
        coords.append( (a, b) )
        if v: print(f"mirror x: ({x}, {y}) -> ({a}, {b})")
    if v: print("-" * 20)
    # Mirror across y axis 
    rg = range(len(coords)-1-oddDiameter, -1+oddDiameter, -1)
    for i in rg:
        x, y = coords[i]
        a = x
        b = d - y - 1
        coords.append( (a, b) )
        if v: print(f"mirror y: ({x}, {y}) -> ({a}, {b})") 
    return coords

# Goes through circles from the defined range and saves them with a delay in
# between to monitor that they all look correct, and will print if there
# if a repeated coordinate anywhere in the set
def check_circles():
    for i in range(4, 30):
        viz = PathVisualizer(i, i)
        circ = right_circle(i)
        viz.savePath(circ, "rightCircle", v=False)
        overlap: bool = viz.checkOverlap(circ, False)
        output = str(i)
        if overlap: output += " overlap detected"
        print(output)
        time.sleep(0.8)

    
# h = 17
# viz = PathVisualizer(h, h, wait=0.6)

# circ = right_circle(h, True)
# viz.savePath(circ, "rightCircle")
# # viz.animatePath(circ, "animateCircle")
# viz.checkOverlap(circ)

check_circles()