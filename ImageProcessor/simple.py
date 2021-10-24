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

def squared_loss_alg(img, save=False):
    pass

if __name__ == '__main__':
    sys.argv.append('.\\ImageProcessor\images\sample04.jpg')

    # Check correct arguments
    if (len(sys.argv) != 2):
        warning('Usage: python simple path\\to\\image.jpg')
        exit()

    # Check correct file
    file_path: str = os.path.abspath(sys.argv[1])
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

    max_difference = np.max(loss_total)
    print(f'max calculated loss was {max_difference}')

    IMAGES = 20
    increment_size = max_difference / IMAGES
    for i in range(5, IMAGES-2):
        thresh = int(increment_size*i)
        temp = (loss_total > thresh) * 255
        # temp = cv2.threshold(loss_total, thresh, 255, cv2.THRESH_BINARY)[1]
        save_path = os.path.join(folder, f'output\\blank_{i:02d}.jpg')
        cv2.imwrite(save_path, temp)

    line_drawing = create_line_drawing_image(img)
    cv2.imwrite(os.path.join(folder, f'output\\line_drawing\\5line_drawing.jpg'), line_drawing)