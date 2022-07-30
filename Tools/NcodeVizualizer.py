from Tools.NcodeParser import parse_ncode
from Tools.Encodings import NCODE_MOVE
import os
import sys
import cv2
import numpy as np
import math

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


def unrotate_points(coordinates: list) -> list:
    '''
    Unrotates all the coordinates in the list by 45 degrees since ncode files 
    are saved at a 45 degree angle to compensate for hardware. 
    '''
    new_coordinates = []
    for element in coordinates:
        if type(element) == str:
            assert(element == NCODE_MOVE)
            new_coordinates.append(element)
        elif type(element) == tuple:
            x, y = element
            length = math.sqrt(x**2 + y**2)
            if x == 0:
                angle = math.radians(90)
            elif x < 0:
                angle = math.radians(180) + math.atan(y/x)
            else:
                angle = math.atan(y/x)
            new_angle = angle - math.radians(45)
            new_x = length * math.cos(new_angle)
            new_y = length * math.sin(new_angle)
            new_coordinates.append((new_x, new_y))
        else:
            print("[WARNING] Unexpected type while translating to " +
                  f"ncode: {type(element)} {element}")
    return new_coordinates

def vizualize_ncode(ncode_file: str):
    # Open and read file
    coords: list = unrotate_points(parse_ncode(ncode_file))
    img: np.ndarray = coordinates_to_image(coords)
    return img


def show_ncode(ncode_file: str):
    '''
    Vizualizes the ncode then opens a new window showing the image
    '''
    img = vizualize_ncode(ncode_file)
    name = os.path.basename(ncode_file)[:-6]
    cv2.imshow(name, img)
    cv2.waitKey(0)
