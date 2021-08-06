import serial
import time
from struct import *

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

number_commands: int = 5

def arduinoRead():
    if arduino.in_waiting > 0:
        while(arduino.in_waiting > 0):
            readStr: str = arduino.readline().decode("utf-8").rstrip()
            print(f"{bcolors.OKCYAN}[ARDUINO] {readStr}{bcolors.ENDC}")
    else:
        print("Nothing to read.")

# Initialize serial port for communication with arduino
arduino = serial.Serial(port='COM4', baudrate=9600, timeout=.1)
print("Waiting...")
while(arduino.in_waiting < 1):
    pass

arduinoRead()
print("Starting")
for i in range(number_commands):
    command: bytes = pack("B", i)
    assert(len(command) == 1)
    arduino.write(command)
    time.sleep(0.8)
    arduinoRead()
