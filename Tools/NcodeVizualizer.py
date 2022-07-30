from Tools.NcodeParser import parse_ncode
from Tools.Encodings import NCODE_MOVE
import os
import sys
import cv2
import numpy as np

# Color to make lines when the pen is moving (off the paper)
COLOR_MOVE = (60, 60, 60)
COLOR_DRAW = (255, 0, 0)  # Color to make lines when the pen is down and drawing


def coordinates_to_image(coordinates: list):
    '''
    Takes a list of ncode formatted commands
    Returns the image in a numpy array of pixels
    '''
    dst: np.ndarray = np.zeros((1200, 800, 3), np.uint8)
    coordinates_onto_image(coordinates, dst)
    return dst


def coordinates_onto_image(coordinates: list, img: np.ndarray, color = COLOR_DRAW, prev_point = (0, 0)):
    '''
    Takes list of ncode formatted coordinates, an image, and the color to draw
    Prev point is the starting point (where the pen would be before starting this set of coordinates)
    Draws the coordinates in that color on the image
    '''
    assert(len(color) == 3)
    prev_point = tuple(int(c) for c in prev_point)
    move: bool = False
    active_color = color
    for coord in coordinates:
        if coord == NCODE_MOVE:
            move = True
        else:
            assert(len(coord) == 2), f"expected {coord} to be coordinates"
            x, y = int(coord[0]), int(coord[1])
            if move:
                active_color = COLOR_MOVE
                move = False
            else:
                active_color = color
            cv2.line(img, prev_point, (x, y), active_color, 1)
            prev_point = (x, y)
    return prev_point


def vizualize_ncode(ncode_file: str):
    # Open and read file
    coords: list = parse_ncode(ncode_file)
    img: np.ndarray = coordinates_to_image(coords)
    return img


def show_ncode(ncode_file: str):
    '''
    Vizualizes the ncode then opens a new window showing the image
    '''
    img = vizualize_ncode(ncode_file)
    name = os.path.basename(ncode_file)
    cv2.imshow(name, img)
    cv2.waitKey(0)


if __name__ == '__main__':
    '''
    Takes a ncode file as an argument and saves an image of the intended output
    '''

    # Check correct arguments
    if (len(sys.argv) < 2):
        print('Usage: python svg_parser.py path\\to\\file.svg path\\to\\save.ncode')
        exit()

    ncode: str = sys.argv[1]

    if (len(sys.argv) < 3):
        # Only provided svg file
        save_path = ncode[:-6] + ".bmp"
        print(f"[SAVE PATH]: {save_path}")
    else:
        save_path = sys.argv[2]

    assert(save_path[-4:] == ".bmp"), f"Expected bmp file {save_path}"
    img = vizualize_ncode(ncode)
    # Save the image
    if cv2.imwrite(save_path, img):
        print(f"Saved image to: {save_path}")
    else:
        print("ERROR SAVING IMAGE")
