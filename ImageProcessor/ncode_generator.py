import os, sys

from image_to_lines import *

default_save_name = "file"

def in_range(x1, y1, px, py, smoothing_factor):
    x_in = px - smoothing_factor <= x1 <= px + smoothing_factor
    y_in = py - smoothing_factor <= y1 <= py + smoothing_factor
    return x_in and y_in

# Takes a list of lines and stores the data into a file
# lines - a list of lines to store
# savename - the name of the file to save
# smoothing_factor - allowed space between lines without picking up pen
def generate_ncode(lines, savename, smoothing_factor = 2):
    # Create the new file for the ncode
    filepath: str = os.path.abspath(savename) + ".ncode"
    file = open(filepath, "x")

    px, py = 0, 0  # prev point ending
    for row in lines:
        x1, y1, x2, y2 = row[0]
        gap = not in_range(x1, y1, px, py, smoothing_factor)

        if gap: file.write("UP\n")
        file.write(f"{x1} {y1}\n")
        if gap: file.write("DN\n")
        file.write(f"{x2} {y2}\n")

    print("Saving to path: " + filepath)

    file.close()



if __name__ == '__main__':
    # Check correct arguments
    if (len(sys.argv) < 3):
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
    