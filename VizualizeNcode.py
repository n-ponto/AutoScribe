'''
Takes a ncode file as an argument and displays an image of the coordinates
drawn onto a canvas.
'''

import argparse
from Tools.NcodeVizualizer import show_ncode

parser = argparse.ArgumentParser(
    description="Show an image of an Ncode File")
parser.add_argument('filepath', type=str,
                    help='path to the NCODE file')

args = parser.parse_args()
show_ncode(args.filepath)
