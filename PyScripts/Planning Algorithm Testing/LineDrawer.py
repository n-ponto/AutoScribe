from PathVisualizer import PathVisualizer
import time

# function for line generation
def bresenhamN(x1,y1,x2, y2) -> list:
    coords = []
    steep: bool = abs(y2-y1) > abs(x2-x1)

    # x =  min(y1, y2) if steep else min(x1, x2)
    # y =  min(x1, x2) if steep else min(y1, y2)
    # gx = max(y1, y2) if steep else max(x1, x2)
    # gy = max(x1, x2) if steep else max(y1, y2)

    print("starting", x1, y1, x2, y2)
    
    switch = False
    if (steep):
        y = min(x1, x2)
        if (y == y1):
            x, gx, gy = y1, y2, x2
        else:
            x, gx, gy = y2, y1, x1
            switch = True
    else:
        x = min(x1, x2)
        if (x == x1):
            gx, y, gy = x2, y1, y2
        else:
            print("case", y1, x2, y2)
            y, gx, gy = y2, x1, y1
            switch = True

    m_new = 2 * abs(gy - y)
    x_diff = gx - x
    slope_error_new = m_new - x_diff
    print("ending", x, y, gx, gy)

    while (x <= gx):
        if steep:
            coords.append((y, x))
        else:
            coords.append( (x, y) )

        # Add slope to increment angle formed
        slope_error_new = slope_error_new + m_new

        # Slope error reached limit, time to
        # increment y and update slope error.
        if (slope_error_new >= 0):
            y = y-1 if switch else y+1
            slope_error_new = slope_error_new - 2 * x_diff
        x+=1

    return coords  

def bresenham(x1,y1,x2, y2):

	m_new = 2 * (y2 - y1)
	slope_error_new = m_new - (x2 - x1)

	y=y1
	for x in range(x1,x2+1):
	
		print("(",x ,",",y ,")\n")

		# Add slope to increment angle formed
		slope_error_new =slope_error_new + m_new

		# Slope error reached limit, time to
		# increment y and update slope error.
		if (slope_error_new >= 0):
			y=y+1
			slope_error_new =slope_error_new - 2 * (x2 - x1)


def bresenhamFromHere(xIn, yIn):
    moves = []
    down = yIn < 0
    back = xIn < 0
    steep: bool = abs(yIn) > abs(xIn)

    ydiff = -1 if down else 1
    xdiff = -1 if back else 1

    # Set the actual output
    changeVal: tuple = xdiff, ydiff
    if (steep):
        changeOne = 0, ydiff
    else:
        changeOne = xdiff, 0
    
    # Switch x and y for the algorithm
    if (steep):
        x, y = yIn, xIn
        xdiff, ydiff = ydiff, xdiff # switch directions
    else:
        x, y = xIn, yIn

    m_new = 2 * abs(y)
    absx = abs(x)
    slope_error_new = m_new - absx
    c = slope_error_new - absx
    for i in range(absx):
        if (slope_error_new >= 0): # change "y"
            moves.append(changeVal)
            slope_error_new += c
        else: # don't change "y"
            moves.append(changeOne)
            slope_error_new += m_new
    
    return moves


def movesToPath(moves: list, startPoint: tuple):
    x, y = (startPoint)
    x = int(x)
    y = int(y)
    coords = []
    coords.append((x, y))
    for dX, dY in moves:
        x += dX
        y += dY
        coords.append((x, y))
    return coords

def getPath(pv: PathVisualizer, startPoint: tuple):
    cx = int(startPoint[0])
    cy = int(startPoint[1])
    coords = []
    print(f"starting from ({cx}, {cy})")
    coords.append((cx, cy))
    pv.savePath(coords, "line")
    ui = ""
    while True:
        ui = input("enter new command: ")
        if (ui == "q"): break
        ix, iy = [int(s) for s in ui.split()]
        moves = bresenhamFromHere(ix, iy)
        for mx, my in moves:
            cx += mx
            cy += my
            print(cx, cy)
            coords.append((cx, cy))
        print(coords)
        pv.savePath(coords, "line")

def testPath(pv: PathVisualizer, startPoint: tuple):
    cx = int(startPoint[0])
    cy = int(startPoint[1])
    coords = []
    print(f"starting from ({cx}, {cy})")
    coords.append((cx, cy))
    pv.savePath(coords, "line")
    # up/down -- right/left -- steep/shallow
    # up right steep, up right shallow, up left steep, up left shallow
    # down right steep, down right shallow, down left steep, down left shallow
    commands = [
        (5, 10), (-20, -6), (5, -23), (20, 12), (-5, 14), (-14, 9), (-4, -16), (12, -4) 

    ]
    for ix, iy in commands:
        print("Command:", ix, iy)
        moves = bresenhamFromHere(ix, iy)
        sx, sy = coords[len(coords)-1]
        for mx, my in moves:
            cx += mx
            cy += my
            print(cx, cy)
            coords.append((cx, cy))
        lx, ly = coords[len(coords)-1]
        assert (sx + ix == lx and sy + iy == ly)
        print(coords)
        pv.savePath(coords, "line")
        time.sleep(0.6)
            

# driver function
if __name__=='__main__':
    h = 40
    viz = PathVisualizer(h, h, wait=0.8)

    # coords = bresenhamN(0, 0, 20, 20)
    # print(coords)

    # moves = bresenhamFromHere(10 , 10)
    # coords = movesToPath(moves, (h/2, h/2))
    # print(coords)
    # viz.savePath(coords, "line")
    testPath(viz, (h/2, h/2))



