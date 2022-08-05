import os, sys
sys.path.append(os.path.dirname(__file__) + "/..")
from Tools.SerialPort import SerialPort
from Tools.NcodeVizualizer import show_ncode


'''
Takes a ncode file as an argument and saves an image of the intended output
'''

# Check correct arguments
if (len(sys.argv) < 2):
    print('Usage: python svg_parser.py path\\to\\file.svg path\\to\\save.ncode')
    exit()

ncode: str = sys.argv[1]

if (len(sys.argv) < 3):
    # Only provided svg file
    save_path = ncode[:-6] + ".bmp"
    print(f"[SAVE PATH]: {save_path}")
else:
    save_path = sys.argv[2]

if save_path[-4:] != ".bmp":
    print(f"[ERROR] save path should be to .bmp file, not: {save_path}")
img = show_ncode(ncode)
# # Save the image
# if cv2.imwrite(save_path, img):
#     print(f"Saved image to: {save_path}")
# else:
#     print("ERROR SAVING IMAGE")