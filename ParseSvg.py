'''
Takes a SVG file as an argument and parse out the lines to draw
Either input a save path for the ncode, or just "viz" to save an image
'''
import os
import argparse
import cv2
from Tools.SvgParser import svg_to_ncode, VizMode, viz_svg

parser = argparse.ArgumentParser(description="Convert an SVG file into NCODE coordinates.")
parser.add_argument('filepath', type=str, help='path to the SVG file')
parser.add_argument('-v', '--viz', type=str, choices=['whole', 'paths', 'parts'],
                    help="Vizualization mode to run in, will change color by " + "either the whole image, the individual paths, or each part")
parser.add_argument('-s', '--savepath', type=str, help="The save path for the NCODE file.")


args = parser.parse_args()

if args.viz is None:
    print("viz is none")

svg_path: str = args.filepath
assert (svg_path[-4:].lower() == ".svg"), "filepath must be to SVG file."

save_path: str = args.savepath

# Save path provided
if args.viz is None:
    if save_path is None:
        save_path = svg_path[:-4] + ".ncode"
    assert (save_path[-6:].lower() == ".ncode"), "To convert SVG to NCODE the save path must be to an NCODE file"
    svg_to_ncode(svg_path, save_path)
# Vizualize
else:
    img = None
    # Vizualize whole document
    if args.viz.lower() == 'whole':
        img = viz_svg(svg_path, VizMode.WHOLE)
    # Vizualize seperate paths
    elif args.viz.lower() == 'paths':
        img = viz_svg(svg_path, VizMode.PATHS)
    # Vizualize seperate parts
    elif args.viz.lower() == 'parts':
        img = viz_svg(svg_path, VizMode.PARTS)
    # Display image
    if save_path is None:
        name = os.path.basename(svg_path)[:-4]
        cv2.imshow(name, img)
        cv2.waitKey(0)
    else:
        assert (save_path[-4:].lower() == '.bmp'), \
            f"To save vizualization the save path must be to an .bmp file"
        if not cv2.imwrite(save_path, img):
            print(f"Error saving image {save_path}")
            exit(-1)

print(f"Saved to {save_path}")
