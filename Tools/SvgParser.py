
import sys
import os
import bezier
from xml.dom import minidom as md
import numpy as np
import cv2
import logging
import enum
import math
from svg.path import parse_path
from svg.path.path import Line, Arc, QuadraticBezier, CubicBezier, Move, Close
from .SimplifySegments import simplify_segments
from .NcodeVizualizer import coordinates_onto_image
from .Encodings import NCODE_MOVE


# Set up logging
logging_path = 'svg_parser.log'
if os.path.exists(logging_path):
    os.remove(logging_path)
logger = logging.getLogger(__name__)
logger.setLevel(logging.DEBUG)
logging.basicConfig(format='%(message)s',
                    filename=logging_path, encoding='utf-8')


class VizMode(enum.Enum):
    '''
    Depth level for vizualizing the file
    '''
    WHOLE = 0   # Vizualize the entire SVG in one color
    PATHS = 1   # Make each path it's own color
    PARTS = 2   # Make each part of each path it's own color


class VizualizationAid:
    '''
    Class to help with vizualizing parts of the SVG in different colors
    Stores the entire image and rotates through colors each time a new part 
    is vizualized (drawn onto the canvas)
    '''

    # Colors to use when vizualizing parts of the SVG in different colors
    VIZ_COLORS = [
        ("red",      (0,   0, 255)),
        ("orange",   (0, 200, 255)),
        ("green",    (0, 255,   0)),
        ("blue",     (255,   0,   0)),
        ("purple",   (255,   0, 255))
    ]
    viz_mode: VizMode
    img: np.ndarray = np.zeros((1200, 800, 3), np.uint8)
    _prev_coord = (0, 0)
    _color_idx = 0

    def __init__(self, viz_mode: VizMode):
        self.viz_mode = viz_mode

    def viz_component(self, coords: list):
        ''' Draws the set of coordinates onto the canvas in a new color
        '''
        color_str, color_bgr = self.VIZ_COLORS[self._color_idx]
        logger.debug(f"\tcolor: {color_str}")
        self._prev_coord = coordinates_onto_image(
            coords, self.img, color_bgr, self._prev_coord)
        self._color_idx = (self._color_idx + 1) % len(self.VIZ_COLORS)


def get_svg_doc(filepath: str) -> md.Document:
    '''
    Take the filepath to a SVG file and return the minidom file document
    This parses the SVG file into the XML components (easier to work with)
    '''
    file_ending = filepath[-4:]
    if (file_ending != ".svg"):
        print(f"[WARNING] file must be .svg type, was {file_ending}")
        return None
    return md.parse(filepath)


def get_svg_paths(svg_doc: md.Document) -> list:
    '''
    Takes a SVG in the form of a minidom document
    Extracts just the paths and returns them in a list
    '''
    return [path.getAttribute('d') for path in svg_doc.getElementsByTagName('path')]


def get_bezier_points(points: list):
    '''
    Takes a list of points, where the first and last are endpoints, and
    everything in the middle is a control point
    Calculates the points along the bezier curve, and returns them in a list
    '''
    # Turn the points into a numpy array
    logger.debug("\t****** get_bezier_points ")
    logger.debug(f"\tcalled with points: {points}")
    nodes = np.transpose(np.array(points))
    # Create the bezier object
    degree: int = len(points) - 1
    curve = bezier.Curve(nodes, degree=degree)
    # Figure out how many points we want along the curve
    # Want fewest points possible to represent all pixels in the curve
    diffs = np.sum(np.absolute(np.diff(nodes, axis=1)), axis=1)
    logger.debug(f"\tdiffs {diffs}")
    count = round(np.min(diffs))  # max x/y change
    logger.debug(f"\tcount {count}")
    if count < 2:
        return [points[-1]]
    s_vals = np.linspace(0, 1.0, count)
    # Get the points along the curve from the object
    eval: np.ndarray = curve.evaluate_multi(s_vals)
    # Reformat the eval
    output = []
    prev = points[0]
    for i in range(count):
        x, y = eval[:, i]
        if (round(x) == round(prev[0]) and round(y) == round(prev[1])):
            continue  # Skip copies
        output.append((x, y))
        prev = (x, y)
    logger.debug(f"\tremoved {count - len(output)} copied coordinates")
    logger.debug(f"\toutput size {len(output)}")
    logger.debug(f"\toutput {output}")
    logger.debug("\t" + "*" * 40)
    output[-1] = points[-1]
    return output


def path_to_segments(path_string: list, va: VizualizationAid = None) -> list:
    '''
    Takes the string format of the SVG path object
    Returns a list of the continous line segments, where each segment is a 
    list of points in that segment
    '''
    path = parse_path(path_string)
    vizualizing: bool = va is not None and va.viz_mode == VizMode.PARTS
    output_segments = []
    prev = None
    prev_viz = 0
    current_segment = []
    for part in path:
        if isinstance(part, Move):
            if len(current_segment) > 0:
                prev_viz = 0
                output_segments.append(current_segment)
                current_segment = []
            prev = (part.end.real, part.end.imag)
            current_segment.append((part.end.real, part.end.imag))
        elif isinstance(part, Line):
            assert(prev[0] == part.start.real)
            assert(prev[1] == part.start.imag)
            prev = (part.end.real, part.end.imag)
            current_segment.append(prev)
        elif isinstance(part, Arc):
            assert(False), "Have not created code for this yet"
        elif isinstance(part, QuadraticBezier):
            assert(prev[0] == part.start.real)
            assert(prev[1] == part.start.imag)
            start = (part.start.real, part.start.imag)
            control = (part.control.real, part.control.imag)
            end = (part.end.real, part.end.imag)
            points = get_bezier_points([start, control, end])
            assert(len(points) > 0), "No points returned"
            current_segment.extend(points)
            prev = points[-1]
        elif isinstance(part, CubicBezier):
            assert(prev[0] == part.start.real and prev[1] == part.start.imag), \
                f"prev {prev}\t{part.start.real}, {part.start.imag}"
            start = (part.start.real, part.start.imag)
            control1 = (part.control1.real, part.control1.imag)
            control2 = (part.control2.real, part.control2.imag)
            end = (part.end.real, part.end.imag)
            points = get_bezier_points([start, control1, control2, end])
            assert(points[-1] == end), f"{points}\n{end}"
            current_segment.extend(points)
            prev = points[-1]
        elif isinstance(part, Close):
            assert(prev[0] == part.start.real)
            assert(prev[1] == part.start.imag)
            prev = (part.end.real, part.end.imag)
            current_segment.append(prev)
        if vizualizing:
            va.viz_component(current_segment[prev_viz:])
        prev_viz = len(current_segment)
    if len(current_segment) > 0:
        output_segments.append(current_segment)
    return output_segments


def svg_to_segments(filepath: str, va: VizualizationAid = None) -> list:
    '''
    Take a path to a svg file and parse out the individual line segments
    where each segment is a continous set of points without needing to pick up 
    the pen
    '''
    doc: md.Document = get_svg_doc(filepath)
    if (doc is None):
        return None

    coordinate_strs: list = get_svg_paths(doc)
    doc.unlink()  # Clean up the not needed dom tree

    segments = []  # List for storing outpoint coordiantes

    print(f"Found {len(coordinate_strs)} different paths in the file")
    logger.debug(f"Found {len(coordinate_strs)} different paths in the file")
    viz_paths = va is not None and va.viz_mode == VizMode.PATHS
    for i, path_string in enumerate(coordinate_strs):
        logger.debug(f"Path #{i}")
        path_segments = path_to_segments(path_string, va)
        if path_segments is None:
            print("\t> path_to_coordinates returned none, path not appended")
        else:
            print("\t> path appended to list")
            if viz_paths:
                path_coordinates = segments_to_coordinates(path_segments)
                va.viz_component(path_coordinates)
            segments.extend(path_segments)  # Add the path to the output list
    return segments


def segments_to_coordinates(segments: list) -> list:
    ''' Takes a list of line segments and joins them into a single list of 
    coordinates with move signals between segments.
    Returns: a list of coordinates with move commands between segments
    '''
    output_coordinates = []
    for seg in segments:
        output_coordinates.append(NCODE_MOVE)
        output_coordinates.extend(seg)
    return output_coordinates


def svg_to_coordinates(filepath: str, va: VizualizationAid = None) -> list:
    '''
    Takes the path to a svg file and parses out the coordinates
    Returns a list of ncode coordinates with move commands between
    '''
    segments = svg_to_segments(filepath, va)
    segments = simplify_segments(segments, 5)
    coordinates = segments_to_coordinates(segments)
    # Return to the orgin after drawing
    coordinates.append(NCODE_MOVE)
    coordinates.append((0, 0))
    return coordinates


def rotate_points(coordinates: list) -> list:
    '''
    Rotates all the coordinates in the list by 45 degrees to compensate for the 
    hardware design to draw at a 45 degree angle naturally.
    '''
    new_coordinates = []
    for element in coordinates:
        if type(element) == str:
            assert(element == NCODE_MOVE)
            new_coordinates.append(element)
        elif type(element) == tuple:
            x, y = element
            length = math.sqrt(x**2 + y**2)
            angle = 90 if x == 0 else math.atan(y/x)
            new_angle = angle + math.radians(45)
            new_x = length * math.cos(new_angle)
            new_y = length * math.sin(new_angle)
            new_coordinates.append((new_x, new_y))
        else:
            print("[WARNING] Unexpected type while translating to " +
                  f"ncode: {type(element)} {element}")
    return new_coordinates


def svg_to_ncode(svg_path: str, save_path: str):
    '''
    Takes the path to a SVG file and a save path for the ncode file
    Parses the paths from the SVG file and saves them in ncode representation
    '''
    coordinates: list = svg_to_coordinates(svg_path)
    coordinates = rotate_points(coordinates)

    # Create the new file for the ncode
    if(save_path[-6:] != ".ncode"):
        print("Adding ncode file ending to save path.")
        save_path += ".ncode"
    filepath: str = os.path.abspath(save_path)
    file = open(filepath, "w")

    for element in coordinates:
        if type(element) == str:
            file.write(element + "\n")
        elif type(element) == tuple:
            x, y = element
            x = round(x)
            y = round(y)
            file.write(f"{x} {y}\n")
        else:
            print(
                f"[WARNING] Unexpected type while translating to ncode: {type(element)} {element}")

    print("DONE TRANSLATING: " + svg_path)
    print("INTO NCODE: " + filepath)
    file.close()


def viz_svg(svg_path: str, viz_mode=VizMode.WHOLE) -> np.ndarray:
    '''
    Takes the path to a SVG file and returns an image showing the parsed output
    '''
    va = VizualizationAid(viz_mode)
    coords = svg_to_coordinates(svg_path, va)
    if va.viz_mode == VizMode.WHOLE:
        va.viz_component(coords)
    return va.img


'''
Refrences

Doc of SVG path commands
https://www.w3.org/TR/SVG/paths.html
'''
