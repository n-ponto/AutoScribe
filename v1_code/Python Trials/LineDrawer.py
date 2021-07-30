# function for line generation
def bresenham(x1,y1,x2, y2) -> list:
    coords = []
    m_new = 2 * (y2 - y1)
    slope_error_new = m_new - (x2 - x1)

    y=y1
    for x in range(x1,x2+1):

        print("(",x ,",",y ,")\n")
        coords.append( (x, y) )

        # Add slope to increment angle formed
        slope_error_new =slope_error_new + m_new

        # Slope error reached limit, time to
        # increment y and update slope error.
        if (slope_error_new >= 0):
            y=y+1
            slope_error_new =slope_error_new - 2 * (x2 - x1)




# driver function
if __name__=='__main__':
    x1 = 3
    y1 = 2
    x2 = 15
    y2 = 5
    bresenham(x1, y1, x2, y2)

    #This code is contributed by ash264
