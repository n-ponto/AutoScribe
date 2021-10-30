# Organization of Image Processor

## Overview

### Summary: 
- The image processor accepts a regular image file and converts the image into line art
- The line art is converted into a set of straight lines which can be sent to the line drawer

### Basic process:
- The color image is converted from grayscale
- Edges of the image are found using the **Canny** algorithm
- The lines in the image are found by using the probabilistic **Hough** transform on the edges

---

### Links:

- [Tkinter Photo GUI](https://python.plainenglish.io/creating-a-basic-photoshop-app-with-python-c24181a09f69) uses pillow and tkinter to create a GUI for photo editing.

- [Python Hough Transform](https://opencv24-python-tutorials.readthedocs.io/en/latest/py_tutorials/py_imgproc/py_houghlines/py_houghlines.html)
general usage of the OpenCV probabilistic Hough transform in python.

- [Interactive Hough Transform](https://docs.opencv.org/4.5.3/d3/de6/tutorial_js_houghlines.html) 
web interface for uploading images and seeing the OpenCV output of the Hough Transform.
Code in JavaScript, but the parameters to Canny and Hough algorithms are a bit better.