import sys, os
sys.path.append(os.path.dirname(__file__ ) + "/..")
from Tools.SerialPort import SerialPort
import time
from struct import *

commandIndex = 4

# Initialize serial port for communication with arduino
arduino = SerialPort()
arduino.awaitResponse()
manualStepCommand: bytes = pack("B", commandIndex)
print(f"Sending manual step command bit: {manualStepCommand}")
arduino.write(manualStepCommand)
time.sleep(0.5)
arduino.readStr()

print("Pen up")
time.sleep(1)
arduino.writeByte(0b1100)
time.sleep(0.5)

print("Pen down")
time.sleep(1)
arduino.writeByte(0b0011)
time.sleep(0.5)

numberSteps = 200

directions =  {
    "Up":         0b1000,
    "Down":       0b0100,
    "Left":       0b0010,
    "Right":      0b0001,
    "Up Left":    0b1010,
    "Up Right":   0b1001,
    "Down Left":  0b0110,
    "Down Right": 0b0101
    }

for name in directions:
    print(name)
    time.sleep(0.5)
    for i in range(numberSteps):
        arduino.writeByte(directions[name])
    time.sleep(0.5)

print("Ending")
arduino.writeByte(0b0000)
time.sleep(0.5)
arduino.readStr()


# for i in range(number_commands):
#     command: bytes = pack("B", i)
#     assert(len(command) == 1)
#     print(f"Command: {command}")
#     arduino.write(command)
#     time.sleep(0.8)
#     arduino.read()
#     print()
