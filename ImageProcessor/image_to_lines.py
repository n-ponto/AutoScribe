import os, sys

import cv2
import numpy as np

FILE_TYPES = ['.jpg', '.jpeg', '.png']  # Allowed file types

# Used to print a formatted warning message to the console
def warning(string: str):
    print(f'\033[93m[WARNING] {string}\033[0m')

# Takes a file path and returns the array representing the image
def open_img(path: str):
    # Check correct file
    file_path: str = os.path.abspath(path)
    if not any(file_path.lower().endswith(x) for x in FILE_TYPES):
        warning('can only accept image files')
        exit()
    file_name = os.path.basename(file_path)[:-4]
    folder = os.path.dirname(file_path)

    # Open the file
    print(f'Opening {file_path}')
    img: np.ndarray = cv2.imread(file_path)
    if (img is None):
        warning('Cannot open file')
        exit()
    return img, file_name, folder

# Takes a set of lines puts them into an image
# Parameters:
#   lines - the lines to place in the image
#   shape - the shape (pixel dimensions) of the image
#   color - the color to draw the lines
# Returns: an image (ndarray)
def lines_to_img(lines: np.ndarray, shape, color = (255, 255, 255)) -> np.ndarray:
    assert(shape[2] == 3)  # 3 dimensions for RGB
    dst: np.ndarray = np.zeros(shape, np.uint8)
    for row in lines:
        x1, y1, x2, y2 = row[0]
        startPoint = (x1, y1)
        endPoint = (x2, y2)
        cv2.line(dst, startPoint, endPoint, color)
    return dst

# Takes a color image and returns the set of lines which make up the image
def img_to_lines(img: np.ndarray):
    gray = cv2.cvtColor(img, cv2.COLOR_RGBA2GRAY, 0)  # convert to grayscale

    # Canny algorithm to extract the edges
    edges = cv2.Canny(image=gray, 
                      threshold1=100, # The smallest value between threshold1 and threshold2 is used for edge linking. 
                      threshold2=200, # The largest value is used to find initial segments of strong edges.
                      apertureSize=3,
                      L2gradient=True)

    # Hough algorithm to get line endpoints from the edges
    lines = cv2.HoughLinesP(image=edges, 
                            rho=2,           # Distance resolution of the accumulator in pixels. 
                            theta=np.pi/180, # Angle resolution of the accumulator in radians.
                            threshold=7,     # Accumulator threshold parameter. Only those lines are returned that get enough votes
                            minLineLength=3, # Line segments shorter than that are rejected. 
                            maxLineGap=10)    # Maximum allowed gap between points on the same line to link them.
    return lines


if __name__ == '__main__':
    sys.argv.append('.\\ImageProcessor\images\sample07.jpg')

    # Check correct arguments
    if (len(sys.argv) != 2):
        warning('Usage: python image_to_lines.py path\\to\\image.jpg')
        exit()

    # Open the file
    img: np.ndarray
    folder: str
    img, filename, folder = open_img(sys.argv[1])

    # line_drawing = create_line_drawing_image(img)
    lines = img_to_lines(img)
    lines_img = lines_to_img(lines, img.shape)
    cv2.imwrite(os.path.join(folder, f'{filename}_lines.jpg'), lines_img)