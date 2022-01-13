import sys, os
import bezier
from xml.dom import minidom as md
import numpy as np
import cv2
import matplotlib.pyplot as plt
import logging
import enum

# Set up logging
logging_path = 'svg_parser.log'
if os.path.exists(logging_path): os.remove(logging_path)
logger = logging.getLogger(__name__)
logger.setLevel(logging.DEBUG)
logging.basicConfig(format='%(message)s', filename=logging_path, encoding='utf-8')

# SVG encodings for different commands
MOVE_STR = "MOVE"
CLOSE_PATH = 'Z' # SVG encoding to close the current path (i.e. return to starting point)
LINETO = 'L'
RELATIVE_MOVETO = 'm'
MOVETO = 'M'
HORIZONTAL_LINETO = 'H'
VERTIAL_LINETO = 'V'
QUAD_BEZ_CURVETO = 'Q'
CURVE = 'c'

def get_svg_doc(filepath: str) -> md.Document:
    '''
    Take the filepath to a SVG file and return the minidom file document
    '''
    file_ending = filepath[-4:]
    if (file_ending != ".svg"):
        print(f"[WARNING] file must be .svg type, was {file_ending}")
        return None
    return md.parse(filepath)
    

def get_svg_paths(svg_doc: md.Document) -> list:
    '''
    Takes a SVG document and return the list of path strings in the document
    '''
    return [path.getAttribute('d') for path in svg_doc.getElementsByTagName('path')]

def get_bezier_points(points: list):
    # Turn the points into a numpy array
    logger.debug("\tget_bezier_points " + "*" * 20)
    logger.debug(f"\tcalled with points: {points}")
    nodes = np.transpose(np.array(points))
    # Create the bezier object
    degree: int = len(points) - 1
    curve = bezier.Curve(nodes, degree=degree)
    # Figure out how many points we want along the curve
    count = int(np.min(np.abs(nodes[:, 0] - nodes[:, -1]))) - 1 # min x/y change
    logger.debug(f"\tcount {count}")
    s_vals = np.linspace(0, 1.0, count)
    # Get the points along the curve from the object
    eval: np.ndarray = curve.evaluate_multi(s_vals)
    # Reformat the eval
    output = []
    prev = points[0]
    for i in range(count):
        x, y = eval[:, i]
        if (round(x) == round(prev[0]) and round(y) == round(prev[1])):
            continue # Skip copies
        output.append((x, y))
        prev = (x, y)
    logger.debug(f"\toutput size {len(output)}")
    logger.debug(f"\toutput {output}")
    logger.debug("\t" + "*" * 40)
    return output

class ParseMode(enum.Enum):
    LINETO = 0
    QUAD_BEZ = 1
    CUBE_BEZ = 2

def parse_path(path_string: list) -> list:
    '''
    Takes path_string, the string format of the SVG path object
    Returns a list of ncode commands for that path, or none on failure
    '''
    logger.debug(path_string)
    parts: list = path_string.split(" ")
    assert(parts[0].lower() == 'm')  # First part should be M for move ?
    
    # Each part should be a coordinate pair
    relative = False # Coordinates are relative or absolute
    path_list = []  # List to store the current path
    prev_coord = (0, 0)
    open_coord = None
    mode: ParseMode = ParseMode.LINETO
    i = 0

    # Handle relative move as first element
    if (parts[0] == RELATIVE_MOVETO):
        # First element absolute, rest relative
        coords: list = parts[1].split(",")
        assert(len(coords) == 2)
        x = float(coords[0])
        y = float(coords[1])
        prev_coord = (x, y)
        open_coord = prev_coord
        path_list.append(MOVE_STR)
        path_list.append(prev_coord)
        relative = True
        i = 2

    while i < len(parts):
        part = parts[i]
        prt_fmt = (f"\"{part}\"").ljust(30)
        coord_fmt = str(prev_coord).ljust(25)
        logger.debug(f"PART {prt_fmt} current coord: {coord_fmt} mode: {mode}")
        coords: list = part.split(",")
        ### Handle command
        if len(coords) == 1:
            relative = part.islower()
            cmd = part.upper()
            logger.debug(f"\tcommand {cmd}\trelative {relative}")
            if cmd == LINETO:
                logger.debug("\tlineto")
                mode = ParseMode.LINETO
            elif cmd == CLOSE_PATH:
                path_list.append(open_coord) # Close path -> add the first element again
                open_coord = None
                logger.debug(f"\tclose path adding {path_list[1]}")
            elif cmd == MOVETO:
                path_list.append(MOVE_STR)
                logger.debug("\tmove")
                mode = ParseMode.LINETO
                open_coord = None
            elif cmd == HORIZONTAL_LINETO:
                x = float(parts[i+1]) # X val is next part
                if relative: x += prev_coord[0]
                y = prev_coord[1]
                assert(x > 0 and y > 0), f"({x}, {y}) should be nonzero"
                prev_coord = x, y
                path_list.append(prev_coord)
                i += 1
                logger.debug(f"\thorizontal to {prev_coord}")
            elif cmd == VERTIAL_LINETO:
                y = float(parts[i+1]) # X val is next part
                if relative: y += prev_coord[1]
                x = prev_coord[0]
                assert(x > 0 and y > 0), f"({x}, {y}) should be nonzero"
                prev_coord = x, y
                path_list.append(prev_coord)
                i += 1
                logger.debug(f"\tvertical to {prev_coord}")
            elif cmd == QUAD_BEZ_CURVETO:
                logger.debug(f"\tquadratic bezier curve")
                mode = ParseMode.QUAD_BEZ
            elif cmd == CURVE:
                print("\t> SVG file includes curves.")
                return None
            else:
                print(f"\t> Can't handle command \"{part}\"")
                return None
        ### Handle coordinate
        elif len(coords) == 2:
            ## Line to parse mode
            if mode == ParseMode.LINETO:
                # Found pair of elements (coordinate pair)
                x = float(coords[0])
                y = float(coords[1])
                # If relative then add previous coordinate
                if relative:
                    x+=prev_coord[0]
                    y+=prev_coord[1]
                if not (x > 0 and y > 0):
                    # Handle error on negative 
                    err_str = f"[ERROR] ({x}, {y}) should be nonzero from {part}"
                    logger.error(err_str)
                    print(err_str)
                    return path_list
                prev_coord = (x, y)
                if open_coord is None: open_coord = prev_coord
                path_list.append(prev_coord)  # Append the coordinate pair to the list
                logger.debug(f"\tlineto {prev_coord}")
            ## Quadratic bezier curve mode
            elif mode == ParseMode.QUAD_BEZ:
                control_pt = parts[i].split(",")
                end_pt = parts[i+1].split(",")
                logger.debug(f"\tcontrol point \"{control_pt}\"")
                logger.debug(f"\tend point \"{end_pt}\"")
                for pt in [control_pt, end_pt]:
                    for k in [0, 1]:
                        pt[k] = float(pt[k])
                        if relative:
                            pt[k] += prev_coord[k]
                assert(all(val>0 for val in pt for pt in [control_pt, end_pt])), "Quad bez parameters negative"
                points = get_bezier_points([prev_coord, control_pt, end_pt])
                prev_coord = end_pt
                path_list.extend(points)
                i += 1
            ## Unknown mode
            else:
                print(f"[WARNING] unhandled mode {mode}")
                return None
        else:
            print(f"[WARNING] Can't parse part of path \"{part}\"")
            return None
        i += 1
    return path_list

def parse_svg(filepath: str) -> list:
    '''
    Take a path to a svg file and parse out the coordinate endpoints
    Doesn't return lines, returns coordinates with a move signal between
    '''

    doc: md.Document = get_svg_doc(filepath)
    if (doc is None):
        return None

    coordinate_strs: list = get_svg_paths(doc)
    doc.unlink()  # Clean up the not needed dom tree

    output = []  # List for storing outpoint coordiantes

    print(f"Found {len(coordinate_strs)} different paths in the file")
    for i, path_string in enumerate(coordinate_strs):
        logger.debug(f"Path #{i}")
        path_list = parse_path(path_string)
        if path_list is None:
            print("\t> parse_path returned none, path not appended")
        else:
            print("\t> path appended to list")
            output.extend(path_list)  # Add the path to the output list
    return output

def svg_to_ncode(svg_path: str, save_path: str):
    '''
    Takes the path to a SVG file and a save path for the ncode file
    Parses the paths from the SVG file and saves them in ncode representation
    '''
    coordinates: list = parse_svg(svg_path)

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
            print(f"[WARNING] Unexpected type while translating to ncode: {type(element)}")
    
    print("DONE TRANSLATING: " + svg_path)
    print("INTO NCODE: " + filepath)
    file.close()


if __name__ == '__main__':
    '''
    Takes a SVG file as an argument and parse out the lines to draw
    '''

    # Check correct arguments
    if (len(sys.argv) < 2):
        print('Usage: python svg_parser.py path\\to\\file.svg path\\to\\save.ncode')
        exit()

    svg_path: str = sys.argv[1]

    if (len(sys.argv) < 3):
        # Only provided svg file
        save_path = svg_path[:-4] + ".ncode"
        print(f"[SAVE PATH]: {save_path}")
    else:
        save_path = sys.argv[2]

    # lines: list = parse_svg(svg_path)
    svg_to_ncode(svg_path, save_path)


'''
Refrences

Doc of SVG path commands
https://www.w3.org/TR/SVG/paths.html
'''

