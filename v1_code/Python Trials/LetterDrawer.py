import circleDrawer as circle

def draw_a(height: int):
    d = height/2
    coords: list = []
    coords += circle.right_circle(d)
    coords += (d, d)
    coords += (d, d/2)





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