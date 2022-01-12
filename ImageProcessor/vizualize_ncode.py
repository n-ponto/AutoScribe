import os, sys
import cv2
import numpy as np
sys.path.append(os.path.dirname(__file__) + "/..")
from Tools.Encodings import NCODE_MOVE

colorMove = (100, 30, 30)  # Color to make lines when the pen is moving (off the paper)
colorDraw = (255, 0, 0) # Color to make lines when the pen is down and drawing

def vizualize_ncode(ncode_file: str, save_file: str):
    # Open and read file
    assert(ncode_file[-6:] == ".ncode"), f"Expected ncode file {ncode_file}"
    assert(save_file[-4:] == ".jpg"), f"Expected jpg file {save_file}"
    print("Parsing ncode file...")
    file = open(ncode_file, 'r')
    assert(file is not None), f"Couldn't open file {ncode_file}"
    dst: np.ndarray = np.zeros((1200, 800), np.uint8)
    lines: list = file.readlines()
    move: bool = False
    prevPoint = (0, 0)
    color = colorMove
    for ln in lines:
        ln = ln.strip()
        if ln.upper() == NCODE_MOVE:
            move = True
        else:
            coords = ln.split(' ')
            assert(len(coords) == 2), "expected {ln} to be coordinates"
            x = int(coords[0])
            y = int(coords[1])
            assert(0 <= x and x <= 800)
            assert(0 <= y and y <= 1200)
            if move:
                color = colorMove
                move = False
            else:
                color = colorDraw
            cv2.line(dst, prevPoint, (x,y), color, 1)
            prevPoint = (x, y)
    # Save the image
    if cv2.imwrite(save_file, dst):
        print(f"Saved image to: {save_file}")
    else:
        print("ERROR SAVING IMAGE")

                


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
        save_path = ncode[:-6] + ".jpg"
        print(f"[SAVE PATH]: {save_path}")
    else:
        save_path = sys.argv[2]

    # lines: list = parse_svg(svg_path)
    vizualize_ncode(ncode, save_path)