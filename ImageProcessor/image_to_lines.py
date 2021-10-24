'''
FROM https://blog.pyrospect.com/2019/01/how-to-convert-photo-images-to.html
Uses built in functions to get a border around an image based on the grayscale values
'''
# -*- coding: utf-8 -*-

import os

import cv2
import numpy as np

def to_path(save_path, file_name) -> str:
    file_name += ".jpg"
    out = os.path.join(save_path, file_name)
    print(f"saving {out}")
    return out

def create_line_drawing_image(img, save_path, file_name):
    kernel = np.array([
        [1, 1, 1, 1, 1],
        [1, 1, 1, 1, 1],
        [1, 1, 1, 1, 1],
        [1, 1, 1, 1, 1],
        [1, 1, 1, 1, 1],
        ], np.uint8)
    # Create grayscale image
    img_gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    cv2.imwrite(to_path(save_path, file_name+"_gray"), img_gray)
    # Dilate image
    img_dilated = cv2.dilate(img_gray, kernel, iterations=1)
    cv2.imwrite(to_path(save_path, file_name+"_dilated"), img_dilated)
    # Difference between dilated and gray
    img_diff = cv2.absdiff(img_dilated, img_gray)
    cv2.imwrite(to_path(save_path, file_name+"_diff"), img_diff)
    # Invert diff to get contour
    img_contour = 255 - img_diff
    cv2.imwrite(to_path(save_path, file_name+"_contour"), img_contour)
    # return contour

def convert_images(dir_from, dir_to):
    for file_name in os.listdir(dir_from):
        if file_name.upper().endswith('.JPG'):
            print(file_name)
            img = cv2.imread(os.path.join(dir_from, file_name))

            create_line_drawing_image(img, dir_to, file_name[:-4])

if __name__ == '__main__':
    dir_src = 'C:\\Users\\noahp\\Documents\\Projects\\DrawingMachine\\Image Processor\\images\\source'
    dir_dest = 'C:\\Users\\noahp\\Documents\\Projects\\DrawingMachine\\Image Processor\\images\\destination'
    convert_images(dir_src, dir_dest)