'''
Sends ncode to the device
'''
import os
import logging
import sys
import time
from threading import Thread
from Tools.Encodings import Commands, Drawing, NCODE_MOVE
from Tools.SerialPort import SerialPort

XMIN = -1200
XMAX = 1200
YMIN = -1200
YMAX = 1200


class NcodeSender:

    _serial: SerialPort
    _thread: Thread
    _continueSending: bool = True
    _doneCallback: callable = None

    def __init__(self, serial: SerialPort):
        self._serial = serial

    def sendAsync(self, filepath: str, doneCallback = None) -> bool:
        '''
        Sends the ncode at the file path to the device
        Returns true on success, false otherwise
        '''
        data = NcodeSender._parseFile(filepath)
        if data is None:
            print("[ERROR] failure parsing ncode file")
            return False

        # Start drawing mode
        self._serial.flushRxBuffer()
        self._serial.writeByte(Commands.ENTER_DRAW_MODE)
        time.sleep(0.5)
        try:
            self._continueSending = True
            self._doneCallback = doneCallback
            self._thread = Thread(target=self._sendPoints, args=(data,))
            self._thread.start()
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

    def interrupt(self):
        print("Sending Ncode Interrupted")
        self._serial.readStr()
        # Send emergency stop signal
        self._continueSending = False
        self._serial.flushTxBuffer()
        self._serial.writePoint(Drawing.EMERGENCY_STOP, 0)
        time.sleep(0.2)
        self._serial.writePoint(Drawing.STOP_DRAWING, 0)
        self._serial.flushRxBuffer()
        time.sleep(0.2)
        self._serial.readStr()
        self._thread.join()
        print('Done interrupting')

    def waitForCompletion(self):
        self._thread.join()

    def _sendPoints(self, data: list):
        TX_DELAY = 0.01  # Time in seconds between sending points over serial
        i: int = 0
        # Start by completely filling the buffer
        print(f"Sending a total of {len(data)} coordinate pairs")
        print("Initially filling buffer...")
        self._serial.readStr()
        RX_SZ = 64 / 4
        while i < len(data) and i < RX_SZ and self._continueSending:
            x, y = data[i]
            self._serial.writePoint(x, y)
            i += 1
            time.sleep(TX_DELAY)
        # Batch the rest
        print(f"Done filling buffer i={i}")
        prev = i
        while i < len(data) and self._continueSending:
            # Wait for the low signal
            # print("Waiting for signal...")
            timeout = time.time() + 40
            while self._serial.readByte() != 0xFF:
                if time.time() > timeout:
                    self._serial.readStr()
                    print('TIMEOUT')
                    self._serial.writePoint(Drawing.STOP_DRAWING, 0)
                    return
            # Fill the buffer again
            self._serial.flushRxBuffer()
            while i < len(data) and i < prev+RX_SZ and self._continueSending:
                x, y = data[i]
                self._serial.writePoint(x, y)
                i += 1
                time.sleep(TX_DELAY)
            print(f"Done sending more bytes, {i} total")
            prev = i
        if not self._continueSending:
            print("Interrupted")
            return
        # Finally send end signal
        print('Done sending all coordinates')
        self._serial.flushRxBuffer()
        time.sleep(0.2)
        self._serial.writePoint(Drawing.STOP_DRAWING, 0)
        # Wait for the done drawing signal
        timeout = time.time() + 40
        while self._serial.readByte() != 0xAB and self._continueSending:
            if time.time() > timeout:
                self._serial.readStr()
                print('TIMEOUT')
                return
        # Call the done callback
        if self._doneCallback is not None and self._continueSending:
            print("Calling done callback")
            self._doneCallback()
            print("Done calling done callback")

    @staticmethod
    def _parseFile(filepath: str) -> list:
        '''
        Extracts and returns a list from points from an ncode file
        '''
        if (filepath[-6:] != ".ncode"):
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
                assert (XMIN <= x and x <= XMAX), f"got invalid x value: {x}"
                # y = struct.pack('h', int(coords[0]))
                y = int(coords[1])
                assert (YMIN <= y and y <= YMAX), f'got invalid y value: {y}'

                # Encode the x value
                x &= 0x0000FFFF
                x &= ~Drawing.FLAG_MASK  # Clear bits for flags
                if (movePen):
                    x |= Drawing.MOVE_PEN
                assert (x <= 0x7FFF), f'outside valid range {x}'
                assert (x != Drawing.EMERGENCY_STOP), f'emergency stop in file'

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
    if not ns.sendAsync(sys.argv[1]):
        print("\nThere was an error sending the file")
        exit()
    print("Success!")
