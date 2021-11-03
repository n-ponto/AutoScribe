from typing import Tuple
from PIL import Image
import time
import pathlib
import os

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

class PathVisualizer:

    # Takes the canvas height and width, and an output file name
    def __init__(self, h: int, w: int, saveDirectory: str=None, wait: float=0.5):
        # Set file name
        if saveDirectory is None:
            curDir = pathlib.Path(__file__).parent.resolve()
            saveDirectory = os.path.join(curDir, "images")
            if not os.path.isdir(saveDirectory):
                print("Creating new directory at: " + saveDirectory)
                os.mkdir(saveDirectory)
       
        self._saveDir: str = saveDirectory

        self._dim = (h, w)
        self._wait = wait

    # Checks if there are any duplicate coordinates in the list
    def checkOverlap(self, coords: list, prt=True) -> bool:
        if prt: print("CHECKING FOR OVERLAPS...")
        w, h = self._dim
        canvas = [0 for x in range(w*h)]
        output: str = ""
        detected_overlap = False
        for c in coords:
            # Print new coord red
            prev_hit = False
            if canvas[self._map(c)] : # if already marked
                prev_hit = True
                detected_overlap = True
            else: # mark
                canvas[self._map(c)] = 1

            extra_info: str = ""
            if prev_hit:
                canvas[self._map(c)] = (255, 0, 0)
                extra_info = " ALREADY HIT!"
            
            output += "\n" + str(c) + extra_info
        if (prt != True):
            return detected_overlap
        elif detected_overlap:
            print(output)
        else:
            print("NO OVERLAP DETECTED")
        print("DONE CHECKING FOR OVERLAPS")
        return detected_overlap
    
    # Takes a list of coordinates and a filename and progressively prints the
    # coordinates in the list to the file so that the user can see the 
    # progression.
    def animatePath(self, coords: list, fileName: str):
        h, w = self._dim
        im = Image.new('RGB', (h, w))
        checker_intensity = 20
        checker = lambda x: tuple((int(x%2==0) * checker_intensity) for _ in range(3))
        canvas = [checker(x+y) for x in range(w) for y in range (h)]


        print("ANIMATING...")
        oldC = None
        for c in coords:
            # Print new coord red
            prev_hit = False
            if canvas[self._map(c)][1] > 100: #if already green or white
                prev_hit = True
            else: # make green
                canvas[self._map(c)] = (0, 255, 0)
            
            if oldC is not None: canvas[self._map(oldC)] = (255, 255, 255)

            extra_info: str = ""
            if prev_hit:
                canvas[self._map(c)] = (255, 0, 0)
                extra_info = " ALREADY HIT!"
            
            print(c, extra_info)
            oldC = c
            im.putdata(canvas)
            self._save(im, fileName)
            time.sleep(self._wait)
        # end for
        canvas[self._map(oldC)] = (255, 255, 255)
        im.putdata(canvas)
        self._save(im, fileName)
        print("DONE!")

    # Takes a list of coordinates and saves an image displaying 
    # the path traved by those coordinates
    def savePath(self, coords: list, fileName, v=True):
        h, w = self._dim
        im = Image.new('1', (h, w))
        canvas = [(0, 0, 0) for x in range(h * w)]
        for c in coords:
            x, y = c
            if x >= w or y >= h or x < 0 or y < 0:
                print(f"{bcolors.FAIL}[ERROR] Coordinate ({x}, {y}) is outside the canvas.{bcolors.ENDC}")
            canvas[self._map(c)] = 1
        im.putdata(canvas)
        self._save(im, fileName, v)

#######################################################################
# PRIVATE 

    # Saves the image file 
    def _save(self, im: Image, fileName: str, v=False):
        if not fileName.endswith(".png"): fileName += ".png"
        fullPath: str = os.path.join(self._saveDir, fileName)
        if v: print(bcolors.OKCYAN + "Saving to: \"" + fullPath + "\""  + bcolors.ENDC)
        im.save(fullPath)

    # Maps x and y coordinates to a 1-D list
    def _map(self, c: Tuple[int, int]) -> int:
        _, w = self._dim
        x, y = c
        return x + (w-y-1)*w