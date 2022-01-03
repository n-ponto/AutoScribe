import os
import sys
from cv2 import VariationalRefinement
import numpy as np

from hough_lines import *

DEFAULT_SAVE_NAME = "file"


def in_range(pt, prev, smoothing_factor):
    x_in = prev[0] - smoothing_factor <= pt[0] <= prev[0] + smoothing_factor
    y_in = prev[1] - smoothing_factor <= pt[1] <= prev[1] + smoothing_factor
    return x_in and y_in


def convert_origin_bottom(lines: list, height: int) -> list:
    '''
    The hough lines algorithm calculates line segments with the origin at the top left
    Drawing machine uses origin at bottom left
    This function converts the coordinate points to the drawing machine orientation
    '''
    print(
        f"Converting coordinates for image of height={height}px")

    print("Before:")
    print(lines)
    output = []
    for start, end in lines:
        # Subtract y value from height
        x1, y1 = start
        x2, y2 = end
        y1 = height - y1
        y2 = height - y2

        output.append(((x1, y1), (x2, y2)))
    print("After:")
    print(output)
    return output


def generate_ncode(lines: list, savename: str):
    '''
    Takes a list of lines and stores the data into a file
    lines - a list of lines to store
        format: [ ( (x1, y1), (x2, y2) ), ( (x3, y3), (x4, y4) ), ((...), (...)), ... ]
    savename - the name of the file to save (should end in .ncode)
    '''
    # Create the new file for the ncode
    if(savename[-6:] != ".ncode"):
        print("Adding ncode file ending to save path.")
        savename += ".ncode"
    filepath: str = os.path.abspath(savename)
    file = open(filepath, "x")

    prev = (0, 0)  # prev point ending
    for start, end in lines:
        # if <2 pixels between line segments
        gap = not in_range(start, prev, 2)

        if gap:
            # Gap between lines -> pen should raise before next point
            file.write("MOVE\n")
        file.write(f"{start[0]} {start[1]}\n")
        file.write(f"{end[0]} {end[1]}\n")

    print("Saving to path: " + filepath)


if __name__ == '__main__':
    # Check correct arguments
    if (len(sys.argv) < 2):
        warning('Usage: python ncode_generator.py path\\to\\image.jpg')
        exit()

    if (len(sys.argv) >= 3):
        savename = sys.argv[2]
    else:
        savename = DEFAULT_SAVE_NAME

    img: np.ndarray
    img, _, _ = open_img(sys.argv[1])

    if (img.shape[0] > 1200 or img.shape[1] > 800):
        warning('This image could be too large to draw')

    lines = img_to_lines(img)
    lines = convert_origin_bottom(lines, img.shape[0])
    generate_ncode(lines, savename)