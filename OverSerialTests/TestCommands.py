import sys, os, time
from struct import *
sys.path.append(os.path.dirname(__file__ ) + "/..")
for path in sys.path:
    print(path)
from Tools.SerialPort import SerialPort

number_commands: int = 6

# Initialize serial port for communication with arduino
arduino = SerialPort()
arduino.awaitResponse()
print("Sending command bits...")
for i in range(number_commands):
    command: bytes = pack("B", i)
    assert(len(command) == 1)
    print(f"Command: {command}")
    arduino.write(command)
    time.sleep(0.8)
    arduino.readStr()
    print()
