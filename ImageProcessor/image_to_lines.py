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
def lines_to_img(lines, shape, color = (255, 255, 255)) -> np.ndarray:
    print(f"Writing {len(lines)} into an image")
    dst: np.ndarray = np.zeros(shape, np.uint8)
    for row in lines:
        startPoint, endPoint = row
        cv2.line(dst, startPoint, endPoint, color)
    return dst

def organize_lines(lines, smoothing_factor = 2):
    '''
    1. Start the search-point at the origin, organized list starts empty
    2. calculate number of steps to each segment endpoint
        (number steps = maximum x or y change from current point given endpoint)
    3. The closest segment is appended to the end of the organized list
    4. The endpoint of the segment is the new search-point for the next loop
    5. Continue steps 2-4 until all line segments have been appended to the organized list
    '''
    organized_lines = [] # retuned array

    prev = (0, 0)  # init prev to origin

    # Find closest point
    while lines.size > 0:
        diffs = np.abs(lines[:][:] - [prev, prev])  # Calculate difference to all endpoints
        # print(f"shape diffs {diffs.shape}")
        # print(diffs[0])
        assert(diffs.shape == lines.shape)
        max_diff = np.max(diffs, axis=2)  # Max diff (x or y value) for each endpoint
        # print(f"shape maxdiff {max_diff.shape}")
        # print(max_diff[0])
        assert(max_diff.shape == (lines.shape[0], 2))

        # Get min distance
        minidx = np.unravel_index(np.argmin(max_diff), max_diff.shape)
        # print(f"min index {minidx}\t{lines[minidx[0]][0]} {lines[minidx[0]][1]}")

        # Check if flip line needed
        segment = lines[minidx[0]]
        if (minidx[1]): # Second endpoint was closer than the first
            segment = (lines[minidx[0]][1], lines[minidx[0]][0])
        print(f"segment: {segment[0]} -> {segment[1]}\tdist={max_diff[minidx]}")
        organized_lines.append(segment)
        prev = segment[1]  # Next search-point starts at segment endpoint
        
        # Remove segment from list
        lines = np.delete(lines, minidx[0], 0)  # Delete the min index from lines

    print(f"Done organizing {len(organized_lines)} lines")
    return organized_lines

# Takes lines array output
def to_numpy(lines) -> np.ndarray:
    arr = []
    print(f"converting {len(lines)} line segments")
    for row in lines:
        x1, y1, x2, y2 = row[0]
        line_segment = ((x1, y1), (x2, y2))
        arr.append(line_segment)
    
    return np.asarray(arr, dtype=np.intc) # Long data type

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
    
    formatted_lines = to_numpy(lines) # Reformat weird output of hough lines algo
    return organize_lines(formatted_lines)


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
    file_path =os.path.join(folder, f'{filename}_lines.jpg')
    print(f"Saving to file {file_path}")
    cv2.imwrite(file_path, lines_img)