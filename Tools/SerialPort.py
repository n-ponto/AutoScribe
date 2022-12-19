import serial
from serial import SerialException
import sys, os, time
import struct
sys.path.append(os.path.dirname(__file__ ) + "/..")
from Tools.Encodings import ConsoleColors as cc

class SerialPort():
    ''' Logic for communicating with the Arduino over the serial connection.
    '''

    _port: serial.Serial

    def __init__(self, port='COM3', baudrate=9600, timeout=1) -> None:
        try:
            self._port = serial.Serial(port=port, baudrate=baudrate, timeout=timeout)
            self.buffer_size = 12800
            self._port.set_buffer_size(rx_size = self.buffer_size, tx_size=self.buffer_size)
        except SerialException:
            print('WARNING: could not connect to Arduino')
            self._port = None

    def awaitResponse(self) -> None:
        ''' Blocks until receives initialization output from the Arduino. Returns
        when it has connected.
        '''
        print("Waiting to hear from Arduino...")
        if self._port is None:
            return
        while(self._port.in_waiting < 1):
            pass
        time.sleep(0.5)
        self.readStr()

    def readStr(self) -> None:
        ''' Reads and prints all strings received.
        '''
        if self._port is None:
            return
        if self._port.in_waiting > 0:
            while(self._port.in_waiting > 0):
                readStr: str = self._port.readline().decode("utf-8").rstrip()
                print(f"{cc.OKCYAN}[ARDUINO] {readStr}{cc.ENDC}")
        else:
            print("Nothing to read.")
            return 
            
    def readByte(self) -> int:
        ''' Reads a single byte and returns it as an int. Returns None if there
        is no byte avaliable.
        '''
        if self._port.in_waiting > 0:
            byte = int.from_bytes(self._port.read(), 'big')
            return byte
        return None

    def write(self, x: bytes) -> None:
        ''' Writes a list of bytes
        '''
        assert(type(x) == bytes)
        self._port.write(x)

    def writeByte(self, x: int, prt=False) -> None:
        ''' Writes a single byte 
        Args:
            x: the byte value as an int
            prt: if the hex value of the byte (x) should be written
        '''
        if self._port is None:
            return
        assert(type(x) == int)
        assert(0 <= x <= 255)
        if prt: print("writing:", hex(x))
        self._port.write(struct.pack("B", x))

    def writeShort(self, x: int, prt=False):
        ''' Writes an unsigned short (i.e. 2 bytes)

        Args:
            x: the short value as an int
            prt: if the hex value of the short (x) should be written
        '''
        if self._port is None:
            return
        assert(type(x) == int)
        assert(0 <= x <= 65535)
        if prt: print("writing:", hex(x))
        self._port.write(struct.pack("H", x))

    def writePoint(self, x:int, y:int, prt=False):
        ''' Writes a pair of unsigned shorts (i.e. 4 bytes total)
        Args:
            x: the short x value as an int
            y: the short y value as an int
            prt: if the hex value of the shorts should be written
        '''
        assert(type(x)==int and type(y)==int)
        if prt: print("writing: (", x, ",", y, ")")
        try:
            self._port.write(struct.pack("HH", x, y))
        except Exception as e:
            print(e)
            print(f"({x}, {y})")
            exit()

    def flushTxBuffer(self):
        ''' Clears the transmit buffer'''
        self._port.reset_output_buffer()

    def flushRxBuffer(self):
        ''' Clears the receiving buffer'''
        self._port.reset_input_buffer()

if __name__=='__main__':
    sp = SerialPort()
    sp.awaitResponse()

