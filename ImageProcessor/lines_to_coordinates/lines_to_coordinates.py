import numpy as np

THRESH = 255 / 2


def lines_to_coordinates(lines: np.ndarray) -> list:
    coordinates = list()
    x, y = np.unravel_index(np.argmax(lines, axis=None), lines.shape)
    coordinates.append((x, y))

    # Look around point for another dark spot
    for i in range(-1, 1):
        for j in range(-1, 1):
            if i == j == 0: continue
            # If we find one
            # Check if follows slope of rest of line

    # If found another point that follows slope continue
    # If can't find another that follows slope, start new line from current point
    # If can't find line from current point, look for new starting point

    return coordinates


'''
https://docs.opencv.org/4.5.3/d3/de6/tutorial_js_houghlines.html
^ Has sample code to get lines from images

let src = cv.imread('canvasInput');
let dst = cv.Mat.zeros(src.rows, src.cols, cv.CV_8UC3);
let lines = new cv.Mat();
let color = new cv.Scalar(255, 0, 0);
cv.cvtColor(src, src, cv.COLOR_RGBA2GRAY, 0);
cv.Canny(src, src, 50, 200, 3);
// You can try more different parameters
cv.HoughLinesP(src, lines, 1, Math.PI / 180, 2, 0, 0);
// draw lines
for (let i = 0; i < lines.rows; ++i) {
    let startPoint = new cv.Point(lines.data32S[i * 4], lines.data32S[i * 4 + 1]);
    let endPoint = new cv.Point(lines.data32S[i * 4 + 2], lines.data32S[i * 4 + 3]);
    cv.line(dst, startPoint, endPoint, color);
}
cv.imshow('canvasOutput', dst);
src.delete(); dst.delete(); lines.delete();


'''