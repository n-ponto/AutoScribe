from SerialPort import SerialPort
import time
from struct import *

number_commands: int = 5

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
    arduino.read()
    print()
