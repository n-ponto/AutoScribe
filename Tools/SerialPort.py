from Tools.Encodings import ConsoleColors as cc
import serial
from serial import SerialException
from serial.tools import list_ports
import sys
import os
import time
import struct
import bluetooth
import threading
sys.path.append(os.path.dirname(__file__) + "/..")

BLUETOOTH_DEVICE_NAME = "AutoScribe"
HANDSHAKE = 0x55


class ComPort():
    ''' Logic for communicating with the Arduino over the serial connection.'''

    def awaitResponse(self) -> None:
        ''' Blocks until receives initialization output from the Arduino. Returns
        when it has connected.
        '''
        raise NotImplementedError

    def readStr(self) -> None:
        ''' Reads and prints all strings received.'''
        raise NotImplementedError

    def readByte(self) -> int:
        ''' Reads a single byte and returns it as an int. Returns None if there
        is no byte avaliable.'''
        raise NotImplementedError

    def write(self, x: bytes) -> None:
        ''' Writes a list of bytes
        '''
        raise NotImplementedError

    def writeByte(self, x: int, prt=False) -> None:
        ''' Writes a single byte 
        Args:
            x: the byte value as an int
            prt: if the hex value of the byte (x) should be written
        '''
        raise NotImplementedError

    def writeShort(self, x: int, prt=False):
        ''' Writes an unsigned short (i.e. 2 bytes)

        Args:
            x: the short value as an int
            prt: if the hex value of the short (x) should be written
        '''
        raise NotImplementedError

    def writePoint(self, x: int, y: int, prt=False):
        ''' Writes a pair of unsigned shorts (i.e. 4 bytes total)
        Args:
            x: the short x value as an int
            y: the short y value as an int
            prt: if the hex value of the shorts should be written
        '''
        raise NotImplementedError

    def flushTxBuffer(self):
        ''' Clears the transmit buffer'''
        raise NotImplementedError

    def flushRxBuffer(self):
        ''' Clears the receiving buffer'''
        raise NotImplementedError

    def handshake(self, dataObject):
        ''' Sends a handshake byte to the device to ensure it is ready to receive data.'''
        # Wait for handshake signal from device
        print("Waiting for handshake signal from device...")

        def waitForHandshake():
            while self.readByte() != HANDSHAKE:
                pass
        thread = threading.Thread(target=waitForHandshake)
        thread.start()
        thread.join(5)  # Wait for 5 seconds
        print("Received handshake signal from device.")
        self.flushRxBuffer()
        self.flushTxBuffer()
        self.writeByte(HANDSHAKE)
        print("Sent handshake signal to device.")
        self.writeByte(int(dataObject.PenUpHeight))
        self.writeByte(int(dataObject.PenDownHeight))
        self.writeShort(int(dataObject.MoveSpeed))
        time.sleep(0.5)
        self.readStr()


class SerialPort(ComPort):

    print(serial.__version__)
    _port: serial.Serial

    def __init__(self, port='COM3', baudrate=9600, timeout=1) -> None:
        try:
            self.buffer_size = 12800
            self._initPort(port, baudrate, timeout)
        except SerialException:
            print('WARNING: could not connect to Arduino on port', port)
            self._port = None
            self._tryFindArduino()

    def __del__(self):
        if self._port is not None:
            self._port.close()

    def _tryFindArduino(self) -> str:
        ''' Attempts to find the Arduino by checking all available ports.'''
        ports = list_ports.comports()
        valid_port = None
        # Check all available ports for an Arduino
        for port, desc, _ in ports:
            if "Arduino" in desc:
                print(f"Found Arduino on port {valid_port}: {desc}")
                valid_port = port
                break
        # If no Arduino was found, prompt the user to select a port
        if valid_port is None:
            print("Could not find Arduino.")
            print("Available ports:")
            for i, (port, desc), _ in enumerate(ports):
                print(f"{i}\t{port}\t{desc}")
            idx = int(input("Enter the index of the port to use: "))
            valid_port = ports[idx][0]
        self._initPort(valid_port, 9600, 1)

    def _initPort(self, port: str, baudrate: int, timeout: int) -> serial.Serial:
        ''' Creates a serial port object with the given parameters.'''
        self._port = serial.Serial(port=port, baudrate=baudrate, timeout=timeout)
        self._port.set_buffer_size(rx_size=self.buffer_size, tx_size=self.buffer_size)

    def awaitResponse(self) -> None:
        ''' Blocks until receives initialization output from the Arduino. Returns
        when it has connected.
        '''
        print("Waiting to hear from Arduino...")
        if self._port is None:
            return
        while (self._port.in_waiting < 1):
            pass
        time.sleep(0.5)
        self.readStr()

    def readStr(self) -> None:
        ''' Reads and prints all strings received.'''
        if self._port is None:
            return
        if self._port.in_waiting > 0:
            while (self._port.in_waiting > 0):
                readStr: str = self._port.readline().decode("utf-8").rstrip()
                print(f"{cc.OKCYAN}[ARDUINO] {readStr}{cc.ENDC}")
        else:
            print("Nothing to read.")
            return

    def readByte(self) -> int:
        ''' Reads a single byte and returns it as an int. Returns None if there
        is no byte avaliable.'''
        if self._port.in_waiting > 0:
            byte = int.from_bytes(self._port.read(), 'big')
            return byte
        return None

    def write(self, x: bytes) -> None:
        ''' Writes a list of bytes.'''
        assert (type(x) == bytes)
        self._port.write(x)

    def writeByte(self, x: int, prt=False) -> None:
        ''' Writes a single byte 
        Args:
            x: the byte value as an int
            prt: if the hex value of the byte (x) should be written
        '''
        if self._port is None:
            return
        assert (type(x) == int)
        assert (0 <= x <= 255)
        if prt:
            print("writing:", hex(x))
        self._port.write(struct.pack("B", x))

    def writeShort(self, x: int, prt=False):
        ''' Writes an unsigned short (i.e. 2 bytes)

        Args:
            x: the short value as an int
            prt: if the hex value of the short (x) should be written
        '''
        if self._port is None:
            return
        assert (type(x) == int)
        assert (0 <= x <= 65535)
        if prt:
            print("writing:", hex(x))
        self._port.write(struct.pack("H", x))

    def writePoint(self, x: int, y: int, prt=False):
        ''' Writes a pair of unsigned shorts (i.e. 4 bytes total)
        Args:
            x: the short x value as an int
            y: the short y value as an int
            prt: if the hex value of the shorts should be written
        '''
        assert (type(x) == int and type(y) == int)
        if prt:
            print("writing: (", x, ",", y, ")")
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


class BluetoothPort(ComPort):
    """Communicates with microcontroller over Bluetooth module."""

    _address: str
    _socket: bluetooth.BluetoothSocket = bluetooth.BluetoothSocket(bluetooth.RFCOMM)

    def __init__(self, address=None, baudrate=9600, timeout=1) -> None:
        if address is None:
            self._address = self._search_for_address()
        else:
            self._address = address

        port, name, host = self._find_device()
        self._socket.connect((host, port))
        print(f'Connected to \"{name}\" {host} on {port}')

    def __del__(self):
        print("Closing Bluetooth socket")
        self._socket.close()

    def _search_for_address(self) -> str:
        print('Searching for nearby devices...')
        nearby_devices = bluetooth.discover_devices(
            duration=2, lookup_names=True, flush_cache=True, lookup_class=False)
        print("Found {} devices".format(len(nearby_devices)))
        device_addr = None
        for addr, name in nearby_devices:
            try:
                print("   {} - {}".format(addr, name))
            except UnicodeEncodeError:
                name = name.encode("utf-8", "replace")
                print("   {} - {}".format(addr, name))
            if name == "AutoScribe":
                device_addr = addr

        if device_addr is None:
            print('Could not find device')
            exit(0)

        return device_addr

    def _find_device(self) -> None:
        print(f'Searching for AutoScribe at {self._address}')
        service_matches = bluetooth.find_service(address=self._address)

        if len(service_matches) == 0:
            print("Couldn't connect to the device.")
            exit(0)

        first_match = service_matches[0]
        port = first_match["port"]
        name = first_match["name"]
        host = first_match["host"]

        return port, name, host

    def awaitResponse(self) -> None:
        pass

    def readStr(self) -> None:
        ''' Reads and prints all strings received.'''
        data = None

        def read_all():
            nonlocal data
            data = self._socket.recv(1024)
        thread = threading.Thread(target=read_all)
        thread.start()
        thread.join(.25)  # Wait for 0.25 seconds
        if not data:
            print("Nothing to read")
            return
        try:
            readStr = data.decode("utf-8").rstrip()
            print(f"{cc.OKCYAN}[ARDUINO] {readStr}{cc.ENDC}")
        except UnicodeDecodeError:
            print(f"{cc.WARNING} Could not decode {data} {cc.ENDC}")

    def readByte(self) -> int:
        ''' Reads a single byte and returns it as an int. Returns None if there
        is no byte avaliable.'''
        data = self._socket.recv(1)
        if not data:
            return None
        byte = int.from_bytes(data, 'big')
        return byte

    def write(self, x: bytes) -> None:
        ''' Writes a list of bytes.'''
        assert (type(x) == bytes)
        self._socket.send(x)

    def writeByte(self, x: int, prt=False) -> None:
        ''' Writes a single byte 
        Args:
            x: the byte value as an int
            prt: if the hex value of the byte (x) should be written
        '''
        assert (type(x) == int)
        assert (0 <= x <= 255)
        if prt:
            print("writing:", hex(x))
        self._socket.send(struct.pack("B", x))

    def writeShort(self, x: int, prt=False):
        ''' Writes an unsigned short (i.e. 2 bytes)

        Args:
            x: the short value as an int
            prt: if the hex value of the short (x) should be written
        '''
        assert (type(x) == int)
        assert (0 <= x <= 65535)
        if prt:
            print("writing:", hex(x))
        self._socket.send(struct.pack("H", x))

    def writePoint(self, x: int, y: int, prt=False):
        ''' Writes a pair of unsigned shorts (i.e. 4 bytes total)
        Args:
            x: the short x value as an int
            y: the short y value as an int
            prt: if the hex value of the shorts should be written
        '''
        assert (type(x) == int and type(y) == int)
        if prt:
            print("writing: (", x, ",", y, ")")
        try:
            self._socket.send(struct.pack("HH", x, y))
        except Exception as e:
            print(e)
            print(f"({x}, {y})")
            exit()

    def flushTxBuffer(self):
        ''' Clears the transmit buffer'''
        pass

    def flushRxBuffer(self):
        ''' Clears the receiving buffer'''
        # self._socket.recv(1024)
        pass
