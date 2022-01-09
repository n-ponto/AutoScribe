'''
Sends ncode to the device
'''
import sys, time
from Tools.Encodings import *
from Tools.SerialPort import SerialPort

XMIN = 0
XMAX = 800
YMIN = 0
YMAX = 800

class NcodeSender:

    _serial: SerialPort

    def __init__(self, serial: SerialPort):
        self._serial = serial

    def _parseFile(self, filepath: str) -> list:
        if(filepath[-6:] != ".ncode"):
            print("[ERROR] NcodeSender.send requires .ncode file")
            return None
        print("Parsing ncode file...")
        file = open(filepath, 'r')
        if (file is None):
            print(f"[ERROR] couldn't open file {filepath}")
            return None
        output = []
        lines: list = file.readlines()
        movePen: bool = False
        for line in lines:
            line = line.strip()
            if line.upper() == NCODE_MOVE:
                movePen = True
            else:
                coords = line.split(' ') # split at a space
                if (len(coords) != 2):
                    print(f"[ERROR] expected two coordinates from line \"{line}\"")
                    print(f"\"was {len(coords)} spaces")
                x = int(coords[0])
                assert(XMIN <= x and x <= XMAX)
                y = int(coords[1])
                assert(YMIN <= y and y <= YMAX)
                
                # Encode the x value
                x &= ~Drawing.FLAG_MASK
                if (movePen):
                    x |= Drawing.MOVE_PEN
                
                # Send the values over serial
                output.append((x, y))
                movePen = False
        print("Done parsing file")
        return output
    
    def send(self, filepath: str) -> bool:
        '''
        Sends the ncode at the file path to the device
        Returns true on success, false otherwise
        '''

        data = self._parseFile(filepath)
        if data is None:
            print("[ERROR] failure parsing ncode file")
            return False

        # Start drawing mode
        self._serial.writeByte(Commands.ENTER_DRAW_MODE)
        time.sleep(0.2)
        self._serial.read()
        input("Press enter to send drawing data...")

        try:
            for x, y in data:
                self._serial.writePoint(x, y)
            # Finally send end signal
            print('Done sending all coordinates')
            self._serial.writePoint(Drawing.STOP_DRAWING, 0)
        except KeyboardInterrupt:
            print("Sending Ncode Interrupted")
            # Send emergency stop signal
            self._serial.flushTxBuffer()
            self._serial.writePoint(Drawing.EMERGENCY_STOP, 0)
            time.sleep(0.2)
            self._serial.read()
            print("Ending process")
            return False
        return True


if __name__ == '__main__':
    # Check correct arguments
    if (len(sys.argv) < 2):
        print('Usage: python NcodeSender.py path\\to\\file.ncode')
        exit()
    sp = SerialPort()
    ns = NcodeSender(sp)
    sp.awaitResponse()
    if not ns.send(sys.argv[1]):
        print("\nThere was an error sending the file")
        exit()
    print("Success!")



            





            


