import sys, os
from xml.dom import minidom as md
import cv2
from hough_lines import warning

MOVE_STR = "MOVE"
CLOSE_PATH = "z" # SVG encoding to close the current path (i.e. return to starting point)
CURVE = "c"

def get_svg_doc(filepath: str) -> md.Document:
    '''
    Take the filepath to a SVG file and return the minidom file document
    '''
    file_ending = filepath[-4:]
    if (file_ending != ".svg"):
        warning(f"file must be .svg type, was {file_ending}")
        return None
    return md.parse(filepath)
    

def get_svg_paths(svg_doc: md.Document) -> list:
    '''
    Take a SVG document and return the list of path strings in the document
    '''
    return [path.getAttribute('d') for path in svg_doc.getElementsByTagName('path')]

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

    print("\nSVG PATHS:")
    for path_string in coordinate_strs:
        print(path_string)
        # Indicate this is a new path and the pen should lift and move
        # (not stay down and draw) to the next coordinate
        parts: list = path_string.split(" ")
        assert(parts[0].lower() == 'm')  # First part should be M for move ?
        
        # Each part should be a coordinate pair
        failed = False # Indicates failure during parsing path
        path_list = [MOVE_STR]  # List to store the current path
        for i in range(1, len(parts)):
            part = parts[i]
            coords: list = part.split(",")\

            # Check for coordinate pair
            if (len(coords) != 2):
                if (len(coords) == 1 and part.lower() == CLOSE_PATH):
                    path_list.append(path_list[1]) # Close path -> add the first element again
                    continue
                elif (part.lower() == CURVE):
                    # TODO: if there's a special character at part[2] (like 'c') then handle that
                    print("\t> Not appending curve to output")
                    failed = True
                    break
                else:
                    warning(f"Expected two coordinates in part \"{part}\" found {len(coords)}, not appending path")
                    failed = True
                    break

            # Found pair of elements (coordinate pair)
            x = round(float(coords[0]))
            y = round(float(coords[1]))
            assert(x > 0)
            assert(y > 0)
            path_list.append((x, y))  # Append the coordinate pair to the list
        if not failed:
            # print("Appending:")
            # print(path_list)
            print("\t> path appended to list")
            output.extend(path_list)  # Add the path to the output list
    print()
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
            file.write(f"{x} {y}\n")
        else:
            warning(f"Unexpected type while translating to ncode: {type(element)}")
    
    print("SAVING TO PATH: " + filepath)
    file.close()


if __name__ == '__main__':
    '''
    Take a SVG file as an argument and parse out the lines to draw
    '''

    # Check correct arguments
    if (len(sys.argv) < 2):
        warning('Usage: python svg_parser.py path\\to\\file.svg path\\to\\save.ncode')
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

