from xml.dom import minidom as md
import cv2
from image_to_lines import *

MOVE_STR = "MOVE"
CLOSE_PATH = "z" # SVG encoding to close the current path (i.e. return to starting point)
CURVE = "c"


def parse_svg(filepath: str) -> list:
    '''
    Take a path to a svg file and parse out the coordinate endpoints

    Doesn't return lines, returns endpoints with a move signal between
    '''

    file_ending = filepath[-4:]
    if (file_ending != ".svg"):
        warning(f"file must be .svg type, was {file_ending}")
        return

    doc: md.Document = md.parse(filepath)
    coordinate_strs = [path.getAttribute('d') for path
                       in doc.getElementsByTagName('path')]
    doc.unlink()  # Clean up the not needed dom tree

    output = []  # List for storing outpoint coordiantes

    print("\nPath strings:")
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
                    print("Not appending curve to output")
                    failed = True
                    break
                else:
                    # TODO: if there's a special character at part[2] (like 'c') then handle that
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
            output.extend(path_list)  # Add the path to the output list

    print("\n Output list:")
    print(output)

    return []


if __name__ == '__main__':
    '''
    Take a SVG file as an argument and parse out the lines to draw
    '''

    # Check correct arguments
    if (len(sys.argv) < 2):
        warning('Usage: python svg_parser.py path\\to\\file.svg')
        exit()

    lines: list = parse_svg(sys.argv[1])
    # Save the image created by the lines
    # cv2.imwrite("./svg_lines.jpg", lines_to_img(lines))
