import serial
import sys, os, time
from struct import *
sys.path.append(os.path.dirname(__file__ ) + "/..")
from Tools.ConsoleColors import ConsoleColors as cc

class SerialPort():

    _port: serial.Serial

    def __init__(self, port='COM3', baudrate=9600, timeout=.1) -> None:
        self._port = serial.Serial(port=port, baudrate=baudrate, timeout=timeout)
        self.buffer_size = 12800
        self._port.set_buffer_size(rx_size = self.buffer_size, tx_size=self.buffer_size)

    # Opens the serial port for communication and awaits the initialization output
    # from the _port. Returns the Serial object
    def awaitResponse(self):
        print("Waiting to hear from Arduino...")
        while(self._port.in_waiting < 1):
            pass
        time.sleep(0.5)
        self.read()

    def read(self):
        if self._port.in_waiting > 0:
            while(self._port.in_waiting > 0):
                readStr: str = self._port.readline().decode("utf-8").rstrip()
                print(f"{cc.OKCYAN}[ARDUINO] {readStr}{cc.ENDC}")
        else:
            print("Nothing to read.")
            return None

    def write(self, x: bytes):
        assert(type(x) == bytes)
        self._port.write(x)

    def writeByte(self, x: int, prt=False):
        assert(type(x) == int)
        assert(0 <= x <= 255)
        if prt: print("writing:", hex(x))
        self._port.write(pack("B", x))

    def writeShort(self, x: int, prt=False):
        assert(type(x) == int)
        assert(0 <= x <= 65535)
        if prt: print("writing:", hex(x))
        self._port.write(pack("H", x))

    def writePoint(self, x:int, y:int, prt=False):
        assert(type(x)==int and type(y)==int)
        if prt: print("writing: (", x, ",", y, ")")
        self._port.write(pack("HH", x, y))

    def flushTxBuffer(self):
        self._port.reset_output_buffer()

if __name__=='__main__':
    sp = SerialPort()
    sp.awaitResponse()

