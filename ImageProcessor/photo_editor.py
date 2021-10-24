'''
FROM https://python.plainenglish.io/creating-a-basic-photoshop-app-with-python-c24181a09f69
- uses pillow and tkinter to create a GUI for photo editing
'''

from PIL import Image, ImageFilter
from PIL.ImageFilter import (
     BLUR, CONTOUR, EDGE_ENHANCE, EDGE_ENHANCE_MORE,
     EMBOSS, FIND_EDGES, SHARPEN
)



if __name__ == "main":
    pass