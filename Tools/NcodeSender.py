'''
Sends ncode to the device
'''
import sys
import time
import math
import struct
from Tools.Encodings import Commands, Drawing, NCODE_MOVE
from Tools.SerialPort import SerialPort

XMIN = -1200
XMAX = 1200
YMIN = -1200
YMAX = 1200

# Set up logging
import logging, os
logging_path = 'svg_parser.log'
if os.path.exists(logging_path):
    os.remove(logging_path)
logger = logging.getLogger(__name__)
logger.setLevel(logging.DEBUG)
logging.basicConfig(format='%(message)s',
                    filename=logging_path, encoding='utf-8')


class NcodeSender:

    _serial: SerialPort

    def __init__(self, serial: SerialPort):
        self._serial = serial

    def send(self, filepath: str) -> bool:
        '''
        Sends the ncode at the file path to the device
        Returns true on success, false otherwise
        '''
        data = NcodeSender._parseFile(filepath)
        if data is None:
            print("[ERROR] failure parsing ncode file")
            return False

        # Start drawing mode
        self._serial.writeByte(Commands.ENTER_DRAW_MODE)
        time.sleep(0.2)
        try:
            self._send_points(data)
        except KeyboardInterrupt:
            print("Sending Ncode Interrupted")
            self._serial.readStr()
            # Send emergency stop signal
            self._serial.flushTxBuffer()
            self._serial.writePoint(Drawing.EMERGENCY_STOP, 0)
            time.sleep(0.2)
            self._serial.readStr()
            print("Ending process")
            return False
        return True

    def _send_points(self, data: list):
        i: int = 0
        # Start by completely filling the buffer
        print(f"Sending a total of {len(data)} coordinate pairs")
        print("Initially filling buffer...")
        self._serial.readStr()
        RX_SZ = 64 / 4
        while i < len(data) and i < RX_SZ:
            x, y = data[i]
            self._serial.writePoint(x, y)
            i += 1
        # Batch the rest
        print(f"Done filling buffer i={i}")
        prev = i
        while i < len(data):
            # Wait for the low signal
            # print("Waiting for signal...")
            timeout = time.time() + 5
            while self._serial.readByte() != 0xFF:
                if time.time() > timeout:
                    self._serial.readStr()
                    print('TIMEOUT')
                    self._serial.writePoint(Drawing.STOP_DRAWING, 0)
                    return
                time.sleep(.2)
            # Fill the buffer again
            # print("Received signal, sending more bytes")
            while i < len(data) and i < prev+RX_SZ:
                x, y = data[i]
                logger.debug(f"{x}, {y}")
                self._serial.writePoint(x, y)
                i += 1
            print(f"Done sending more bytes, {i} total")
            prev = i
        # Finally send end signal
        print('Done sending all coordinates')
        self._serial.writePoint(Drawing.STOP_DRAWING, 0)



    @staticmethod
    def _parseFile(filepath: str) -> list:
        '''
        Extracts and returns a list from points from an ncode file
        '''
        if(filepath[-6:] != ".ncode"):
            print("[ERROR] NcodeSender.send requires .ncode file")
            return None
        print("Parsing ncode file...")
        with open(filepath, 'r') as f:
            lines: list = f.readlines()
        output = []
        movePen: bool = False
        for line in lines:
            line = line.strip()
            if line.upper() == NCODE_MOVE:
                movePen = True
            else:
                coords = line.split(' ')  # split at a space
                if (len(coords) != 2):
                    print("[ERROR] expected two coordinates from " +
                          f"line \"{line}\"")
                    print(f"was {len(coords)} spaces")
                # x = struct.pack("h", int(coords[0]))
                x = int(coords[0])
                assert(XMIN <= x and x <= XMAX), f"got invalid x value: {x}"
                # y = struct.pack('h', int(coords[0]))
                y = int(coords[1])
                assert(YMIN <= y and y <= YMAX), f'got invalid y value: {y}'

                # Encode the x value
                x &= 0x0000FFFF
                x &= ~Drawing.FLAG_MASK  # Clear bits for flags
                if (movePen):
                    x |= Drawing.MOVE_PEN
                assert(x <= 0x7FFF), f'outside valid range {x}'
                assert(x != Drawing.EMERGENCY_STOP), f'emergency stop in file'

                # Send the values over serial
                output.append((x, y))
                movePen = False
        print("Done parsing file")
        print(f"{len(output)} coordinate pairs to send")
        return output


if __name__ == '__main__':
    # Check correct arguments
    import os
    sys.path.append(os.path.dirname(__file__) + "/..")
    sys.argv.append('.\\images\\autoScribeText.ncode')
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
