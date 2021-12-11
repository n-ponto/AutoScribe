"""
Tests using the serial communications chanel to ensure the functionality of the
Drawing Machine code on the Arduino
"""
import os, sys, time
p = os.path.abspath(os.path.join(os.path.dirname(__file__), '../..'))
sys.path.append(p)
from PythonExperiments.Tools.SerialPort import SerialPort
from PythonExperiments.Tools.Encodings import *

def testEnterExitDrawingMode():
    # Set up serial port
    serial = SerialPort()
    serial.awaitResponse()
    
    # Write command to switch to drawing mode
    serial.writeByte(Commands.ENTER_DRAW_MODE, prt=True)
    print('waiting...')
    time.sleep(4)  # Wait
    serial.read()

    input("press enter to leave drawing mode")

    # Switch out of drawing mode
    serial.writePoint(0x7FF0, 0)
    print('waiting...')
    time.sleep(2)
    serial.read()

    print('waiting...')
    time.sleep(2)
    serial.read()

def testDrawTinySquare():
    serial = SerialPort()
    serial.awaitResponse()

    points = [
        (50, 0),
        (50, 50),
        (0, 50),
        (0, 0),
        (0x7FF0, 0)
    ]

    # Write command to switch to drawing mode
    serial.writeByte(Commands.ENTER_DRAW_MODE, prt=True)
    print("sent draw command")
    print("waiting...")
    time.sleep(2)
    serial.read()

    print("writing points")
    for x, y in points:
        serial.writePoint(x, y)
    
    print("waiting...")
    time.sleep(2)
    serial.read()



if __name__ == '__main__':
    testEnterExitDrawingMode()
