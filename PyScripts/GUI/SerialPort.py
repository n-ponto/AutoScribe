import serial
import time
from struct import *
from Config import ConsoleColors as cc

class SerialPort():

    _port: serial.Serial

    def __init__(self, port='COM4', baudrate=9600, timeout=.1) -> None:
        self._port = serial.Serial(port=port, baudrate=baudrate, timeout=timeout)

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

    def write(self, x: bytes):
        assert(type(x) == bytes)
        self._port.write(x)

    def writeByte(self, x: int):
        assert(type(x) == int)
        self._port.write(pack("B", x))

if __name__=='__main__':
    sp = SerialPort()
    sp.awaitResponse()

