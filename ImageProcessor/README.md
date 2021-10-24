# Organization of Image Processor

## Overview

Summary: 
- The image processor accepts a regular image file and converts the image into line art
- The line art is converted into a set of straight lines which can be sent to the line drawer

Input - an image file (.jpg, .png, etc.)

Output - the coordinates to send to the drawing machine

___

## Steps
### 1. Image to Line Drawing

Summary: Converts an image into line art. The line art is represented by a boolean numpy array.

Input - an image file

Output - a boolean numpy array representation of the line art

### 2. Line Drawing to Coordinates

Summary: converts the line drawing into a set of coordinates which represent the lines to draw to recreate the image on paper

Input - boolean numpy array

Output - list of coordinates to draw
