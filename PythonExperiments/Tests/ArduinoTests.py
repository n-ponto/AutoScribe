"""
Tests using the serial communications chanel to ensure the functionality of the
Drawing Machine code on the Arduino
"""

from PythonExperiments.Tools.SerialPort import SerialPort
from PythonExperiments.Tools.Encodings import *
import time

def testEnterExitDrawingMode():
    # Set up serial port
    serial = SerialPort()
    serial.awaitResponse()
    readStr: str = serial.read()
    # Write command to switch to drawing mode
    serial.writeByte(Commands.ENTER_DRAW_MODE)
    time.sleep(0.5)  # Wait
    serial.read()

    # Switch out of drawing mode
    serial.writePoint(0xFF, 0xFF)
    time.sleep(0.5)
    serial.read()


