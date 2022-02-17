'''
Takes an Ncode file and returns the list of coordinates
'''
from Tools.Encodings import NCODE_MOVE

def parse_ncode(ncode_file: str) -> list:
    '''
    Takes the path on an Ncode file and returns the list of coordinates
    '''
    assert(ncode_file[-6:] == ".ncode"), f"Expected ncode file {ncode_file}"
    print("Parsing ncode file...")
    file = open(ncode_file, 'r')
    assert(file is not None), f"Couldn't open file {ncode_file}"
    lines: list = file.readlines()
    output: list = []
    for ln in lines:
        ln = ln.strip()
        if ln.upper() == NCODE_MOVE:
            output.append(NCODE_MOVE)
        else:
            coords = ln.split(' ')
            assert(len(coords) == 2), f"expected {ln} to be coordinates"
            output.append((int(coords[0]), int(coords[1])))
    return output