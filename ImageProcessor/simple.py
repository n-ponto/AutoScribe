'''
Line drawing algorithm

'''
import os, sys

import cv2
import numpy as np

FILE_TYPES = ['.jpg', '.jpeg', '.png']  # Allowed file types
DEBUG = True

def warning(string: str):
    print(f'\033[93m{string}\033[0m')

def create_line_drawing_image(img, save=False):
    kernel = np.array([
        [1, 1, 1, 1, 1],
        [1, 1, 1, 1, 1],
        [1, 1, 1, 1, 1],
        [1, 1, 1, 1, 1],
        [1, 1, 1, 1, 1],
        ], np.uint8)
    # Create grayscale image
    img_gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    # Dilate image
    img_dilated = cv2.dilate(img_gray, kernel, iterations=1)
    # Difference between dilated and gray
    img_diff = cv2.absdiff(img_dilated, img_gray)
    # Invert diff to get contour
    img_contour = 255 - img_diff
    # return contour
    if (save):
        cv2.imwrite(os.path.join(folder, f'output\\line_drawing\\1gray.jpg'), img_gray)
        cv2.imwrite(os.path.join(folder, f'output\\line_drawing\\2dilated.jpg'), img_dilated)
        cv2.imwrite(os.path.join(folder, f'output\\line_drawing\\3diff.jpg'), img_diff)
        cv2.imwrite(os.path.join(folder, f'output\\line_drawing\\4contour.jpg'), img_contour)
    return img_contour

# Code from:
# https://opencv24-python-tutorials.readthedocs.io/en/latest/py_tutorials/py_imgproc/py_houghlines/py_houghlines.html
def img_to_lines_v1(img: np.ndarray):
    cv2.imwrite('1img.jpg', img)
    gray = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)
    cv2.imwrite('2gray.jpg', gray)
    edges = cv2.Canny(gray,50,150,apertureSize = 3)
    cv2.imwrite('3edges.jpg', edges)
    minLineLength = 100
    maxLineGap = 10
    lines: np.ndarray = np.zeros(img.shape)
    lines = cv2.HoughLinesP(
        image=edges, 
        lines=lines, 
        rho=1, 
        theta=np.pi/180, 
        threshold=50, 
        minLineLength=minLineLength, 
        maxLineGap=maxLineGap)
    width = 10

    view: np.ndarray = np.zeros(img.shape)
    for arr in lines:
        for x1,y1,x2,y2 in arr:
            cv2.line(view,(x1,y1),(x2,y2),(0, 0, 255), thickness=width)

    cv2.imwrite('4houghlines.jpg', view)
    return lines

def img_to_lines_v2(img):
    # let src = cv.imread('canvasInput');
    # let dst = cv.Mat.zeros(src.rows, src.cols, cv.CV_8UC3);
    # let lines = new cv.Mat();
    # let color = new cv.Scalar(255, 0, 0);
    # cv.cvtColor(src, src, cv.COLOR_RGBA2GRAY, 0);
    # cv.Canny(src, src, 50, 200, 3);
    # // You can try more different parameters
    # cv.HoughLinesP(src, lines, 1, Math.PI / 180, 2, 0, 0);
    # // draw lines
    # for (let i = 0; i < lines.rows; ++i) {
    #     let startPoint = new cv.Point(lines.data32S[i * 4], lines.data32S[i * 4 + 1]);
    #     let endPoint = new cv.Point(lines.data32S[i * 4 + 2], lines.data32S[i * 4 + 3]);
    #     cv.line(dst, startPoint, endPoint, color);
    # }
    # cv.imshow('canvasOutput', dst);
    # src.delete(); dst.delete(); lines.delete();


# Takes an image and returns an array where each pixel is the calculated
# gradient of that pixel to those adjacent.
def gradient(img: np.ndarray, folder: str) -> np.ndarray:
    # Get image shifted by one pixel top and left
    offset = 1
    base = img[offset:  , offset:  ]
    left = img[offset:  ,  :-offset]
    top  = img[ :-offset, offset:  ]

    # Get difference to adjacent pixels
    diff_left = cv2.absdiff(base, left)
    diff_top = cv2.absdiff(base, top)
    cv2.imwrite(os.path.join(folder, f'output\\diff_left.jpg'), diff_left)
    cv2.imwrite(os.path.join(folder, f'output\\diff_top.jpg'), diff_top)

    # Calculate squared loss of RGB values
    loss_left = np.sum(diff_left**2, axis=2)
    loss_top =  np.sum(diff_top**2, axis=2)
    loss_total = np.maximum(loss_left, loss_top)
    cv2.imwrite(os.path.join(folder, f'output\\loss_left.jpg'), loss_left / np.max(loss_left) * 255)
    cv2.imwrite(os.path.join(folder, f'output\\loss_top.jpg'), loss_top / np.max(loss_top) * 255)
    cv2.imwrite(os.path.join(folder, f'output\\loss_total.jpg'), loss_total / np.max(loss_total) * 255)
    return loss_total

# Takes an image and saves
def save_gradient_images(gradient: np.ndarray, folder: str):
    max_difference = np.max(gradient)
    print(f'max calculated diff was {max_difference}')
    IMAGES = 20
    increment_size = max_difference / IMAGES
    for i in range(5, IMAGES-2):
        thresh = int(increment_size*i)
        temp = (gradient > thresh) * 255
        # temp = cv2.threshold(loss_total, thresh, 255, cv2.THRESH_BINARY)[1]
        save_path = os.path.join(folder, f'output\\blank_{i:02d}.jpg')
        cv2.imwrite(save_path, temp)
    
# Takes a file path and returns the array representing the image
def open_img(path: str):
    # Check correct file
    file_path: str = os.path.abspath(path)
    if not any(file_path.lower().endswith(x) for x in FILE_TYPES):
        warning('can only accept image files')
        exit()
    folder = os.path.dirname(file_path)

    # Open the file
    print(f'Opening {file_path}')
    img: np.ndarray = cv2.imread(file_path)
    if (img is None):
        warning('Cannot open file')
        exit()
    return img, folder

if __name__ == '__main__':
    sys.argv.append('.\\ImageProcessor\images\sample02.jpg')

    # Check correct arguments
    if (len(sys.argv) != 2):
        warning('Usage: python simple path\\to\\image.jpg')
        exit()

    # Open the file
    img: np.ndarray
    folder: str
    img, folder = open_img(sys.argv[1])

    line_drawing = create_line_drawing_image(img)
    lines = img_to_lines_v2(img)