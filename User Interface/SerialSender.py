import serial
import time
from struct import *
from enum import Enum

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

class commandStrings:
    MOVE = ['move', 'goto', 'travel']
    PEN = ['pen', 'angle']
    HOME = ['home', 'reset', 'zero']

class commands:
    MOVE_TO_COORDINATE: chr = 0x00
    CHANGE_PEN_ANGLE: chr =   0x01
    RESET_HOME: chr = 0x02

def printError(text: str):
    print(f"{bcolors.WARNING}[ERROR] {text}{bcolors.ENDC}")

# Initialize serial port for communication with arduino
arduino = serial.Serial(port='COM4', baudrate=9600, timeout=.1)
messageStruct = 'BHH'

def write_coordinates(x: int, y: int):
    message: bytes = pack('BHH', 1, x, y)
    # print(unpack('HH', message))
    arduino.write(message)

def arduinoRead():
    if arduino.in_waiting > 0:
        while(arduino.in_waiting > 0):
            readStr: str = arduino.readline().decode("utf-8").rstrip()
            print(f"{bcolors.OKCYAN}[ARDUINO] {readStr}{bcolors.ENDC}")
    else:
        print("Nothing to read.")

def constructMoveMessage():
    coordStr: str = input("Enter coordinates: ")
    try:
        x, y = [int(s) for s in coordStr.split()]
        if (0 <= x < 1400 and 0 <= y < 1400):
            message: bytes = pack(messageStruct, commands.MOVE_TO_COORDINATE, x, y)
            return message
        else:
            printError(f"({x}, {y}) is not in the valid range.")
    except ValueError:
        printError(f"Could not recognize input as a coordinate: {coordStr}.")
    return None

def constructPenMessage():
    angleStr: str = input("Enter pen angle: ")
    try:
        x = int(angleStr)
        if 30 <= x <= 90:  # allowed angle range for the pen
            message: bytes = pack(messageStruct, commands.CHANGE_PEN_ANGLE, x, 0x00)
            return message
        else:
            printError(f"{x} is not in the valid range.")
    except ValueError:
        printError(f"Could not recognize input as an angle: {angleStr}")
    return None

def constructHomeMessage():
    message: bytes = pack(messageStruct, commands.RESET_HOME, 0x00, 0x00)
    return message

def handleInput():
    instr: str = input("Enter command: ").lower().strip()
    
    if instr == "end" or instr == "quit":
        quit()
    elif instr == "read":
            arduinoRead()
    else:
        message: bytes
        if instr in commandStrings.MOVE:
            message = constructMoveMessage()
        elif instr in commandStrings.PEN:
            message = constructPenMessage()
        elif instr in commandStrings.HOME:
            message = constructHomeMessage()
        else:
            printError("Input didn't match any of the commands.")
            return
        
        if message is None:
            printError("Could not construct message.")
            return

        print(f"Sending message: {unpack(messageStruct, message)}")
        arduino.write(message)
        time.sleep(0.5)
        arduinoRead()


while True:
    handleInput()
    
    