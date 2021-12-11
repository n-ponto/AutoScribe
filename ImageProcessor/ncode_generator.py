import os, sys
import numpy as np

from image_to_lines import *

default_save_name = "file"

def in_range(pt, prev, smoothing_factor):
    x_in = prev[0] - smoothing_factor <= pt[0] <= prev[0] + smoothing_factor
    y_in = prev[1] - smoothing_factor <= pt[1] <= prev[1] + smoothing_factor
    return x_in and y_in

# Takes a list of lines and stores the data into a file
# lines - a list of lines to store
# savename - the name of the file to save
# smoothing_factor - allowed space between lines without picking up pen
def generate_ncode(lines, savename):
    # Create the new file for the ncode
    filepath: str = os.path.abspath(savename) + ".ncode"
    file = open(filepath, "x")

    prev = (0, 0)  # prev point ending
    for start, end in lines:
        gap = not in_range(start, prev, 2)

        if gap: file.write("UP\n")
        file.write(f"{start[0]} {start[1]}\n")
        if gap: file.write("DN\n")
        file.write(f"{end[0]} {end[1]}\n")

    print("Saving to path: " + filepath)

    file.close()



if __name__ == '__main__':
    # Check correct arguments
    if (len(sys.argv) < 2):
        warning('Usage: python ncode_generator.py path\\to\\image.jpg')
        exit()

    if (len(sys.argv) >= 3):
        savename = sys.argv[2]
    else:
        savename = default_save_name

    img: np.ndarray
    folder: str
    img, filename, folder = open_img(sys.argv[1])

    lines = img_to_lines(img)
    generate_ncode(lines, savename)
    