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

TIMEOUT_DELAY = 40  # Delay in seconds before timeout

SIGNAL_START_SENDING = 0xFA  # Signal from device to start sending data
SIGNAL_BUFFER_EMPTY = 0xFF  # Signal from device that buffer is empty
SIGNAL_DONE_DRAWING = 0xAB  # Signal from device that drawing is done


class NcodeSender:

    _serial: SerialPort
    _thread: Thread
    _continueSending: bool = True
    _doneCallback: callable = None

    def __init__(self, serial: SerialPort):
        self._serial = serial

    def sendAsync(self, filepath: str, doneCallback=None) -> bool:
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
        self._serial.readStr()
        try:
            self._continueSending = True
            self._doneCallback = doneCallback
            self._thread = Thread(target=self._sendPoints, args=(data,))
            self._thread.start()
        except KeyboardInterrupt:
            print("KeyboardInterrupt")
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
        self._continueSending = False
        print("Sending Ncode Interrupted")
        self._serial.readStr()
        # Send emergency stop signal
        self._serial.writePoint(Drawing.EMERGENCY_STOP, 0)
        time.sleep(0.2)
        self._thread.join(timeout=5)
        print('Stopped thread, waiting on device...')
        self._wait_for_byte(SIGNAL_DONE_DRAWING)
        self._serial.flushRxBuffer()
        print("Device has stopped.")

    def waitForCompletion(self):
        self._thread.join()

    def _wait_for_byte(self, expected_byte: int) -> bool:
        """Waits for a byte to be received from the device
        Args:
            byte: The byte to wait for
        Returns:
            True if the byte was received, False if timed out
        """
        timeout = time.time() + TIMEOUT_DELAY
        unexpected_buffer = []
        byte = None
        while self._continueSending:
            byte = self._serial.readByte()
            if byte == expected_byte:
                break
            elif byte is not None:
                unexpected_buffer.append(byte)
            elif byte is None and len(unexpected_buffer) > 0:
                print(f"\tunexpected bytes as text: {''.join([chr(x) for x in unexpected_buffer])}")
                unexpected_buffer = []
            if time.time() > timeout:
                break
            time.sleep(0.1)

        if len(unexpected_buffer) > 0:
            print(f"\tunexpected bytes as text: {''.join([chr(x) for x in unexpected_buffer])}")

        if byte == expected_byte:
            return False
        elif time.time() > timeout:
            print('TIMEOUT')
            return True
        return True

    def _sendPoints(self, data: list):
        TX_DELAY = 0.01  # Time in sec between sending individual coordinates
        RX_SZ = int(64 / 4)  # Size of the RX buffer in coordinate pairs
        i: int = 0

        def send_batch():
            '''Sends a batch of data points to the device to fill the RX buffer'''
            nonlocal i
            end_idx = i+RX_SZ
            while i < len(data) and i < end_idx and self._continueSending:
                x, y = data[i]
                self._serial.writePoint(x, y)
                i += 1
                time.sleep(TX_DELAY)

        # Handshake with device before sending data
        print(f"Sending a total of {len(data)} coordinate pairs.")
        self._serial.flushTxBuffer()  # Clear the TX buffer
        self._serial.writeByte(SIGNAL_START_SENDING)
        time.sleep(0.5)
        print("Waiting for start signal from device...")
        if self._wait_for_byte(SIGNAL_START_SENDING):
            return

        print("Received start signal. Sending coordinates...")
        send_batch()
        print(f'Send initial {RX_SZ} bytes.')
        while i < len(data) and self._continueSending:
            # Wait for the low signal from device
            print("Waiting for empty buffer signal from device...")
            if self._wait_for_byte(SIGNAL_BUFFER_EMPTY):
                return
            print(f'Recieved empty buffer signal, sending {RX_SZ} more bytes')

            # Fill the buffer
            self._serial.flushRxBuffer()
            send_batch()
            print(f"Done sending bytes, i={i} total")

        if not self._continueSending:
            print("Interrupted")
            return

        # Finally send end signal
        print('Done sending all coordinates')
        self._serial.flushRxBuffer()
        time.sleep(0.2)
        self._serial.writePoint(Drawing.STOP_DRAWING, 0)

        # Wait for response from device
        print('Waiting for done drawing signal from device...')
        self._wait_for_byte(SIGNAL_DONE_DRAWING)
        print('Received done drawing signal from device!')
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
