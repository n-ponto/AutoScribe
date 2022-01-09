"""
Tests using the serial communications chanel to ensure the functionality of the
Drawing Machine code on the Arduino
"""
import os
import sys
import time
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
    time.sleep(.1)  # Wait
    serial.read()

    input("press enter to leave drawing mode")

    # Switch out of drawing mode
    serial.writePoint(Drawing.EMERGENCY_STOP, 0)
    time.sleep(.1)
    serial.read()


def testEnterExitManualControlMode():
    # Set up serial port
    serial = SerialPort()
    serial.awaitResponse()

    # Write command to switch to manual control mode
    serial.writeByte(Commands.ENTER_MANUAL_CONTROL_MODE, prt=True)
    time.sleep(.1)  # Wait
    serial.read()

    # Switch out of drawing mode
    serial.writeByte(0, prt=True)
    time.sleep(.1)
    serial.read()


def testSetStepperDelay():
    # Set up serial port
    serial = SerialPort()
    serial.awaitResponse()

    # Write command to switch to drawing mode
    serial.writeByte(Commands.SET_STEPPER_DELAY, prt=True)
    time.sleep(.1)  # Wait
    serial.read()

    # Set the delay to 1500
    serial.writeShort(1500, prt=True)
    time.sleep(.5)
    serial.read()


def testSetDelayDrawManualControl():
    # Set up serial port
    serial = SerialPort()
    serial.awaitResponse()

    # Set the delay
    serial.writeByte(Commands.SET_STEPPER_DELAY, True)
    serial.writeShort(1600)
    time.sleep(1)
    serial.read()

    # Enter then exit drawing mode
    serial.writeByte(Commands.ENTER_DRAW_MODE, True)
    serial.writePoint(Drawing.STOP_DRAWING, 0)
    time.sleep(1)
    serial.read()

    # Enter then exit manual control mode
    serial.writeByte(Commands.ENTER_MANUAL_CONTROL_MODE, True)
    serial.writeByte(0)
    time.sleep(1)
    serial.read()


def testSetDelayManualControlMovement():
    # Set up serial port
    serial = SerialPort()
    serial.awaitResponse()

    # Set the delay
    serial.writeByte(Commands.SET_STEPPER_DELAY, True)
    serial.writeShort(1600)
    time.sleep(1)
    serial.read()

    # Enter manual control mode
    serial.writeByte(Commands.ENTER_MANUAL_CONTROL_MODE, True)
    time.sleep(1)
    serial.read()

    # Down, Up, Left, Right
    keys = [5, 3, 7, 9]
    for key in keys:
        serial.writeByte(key)  # press key
        time.sleep(.5)
        serial.writeByte(key+1)  # release key
        time.sleep(1)

    # Leave manual control mode
    serial.writeByte(0, True)
    time.sleep(1)
    serial.read()


def testDrawTinySquare():
    # Set up serial port
    serial = SerialPort()
    serial.awaitResponse()

    # Set the delay
    serial.writeByte(Commands.SET_STEPPER_DELAY, True)
    serial.writeShort(1600)
    time.sleep(1)
    serial.read()

    # Enter draw mode
    serial.writeByte(Commands.ENTER_DRAW_MODE, True)
    time.sleep(1)
    serial.read()

    size = 200
    points = [
        (size, 0),
        (size, size),
        (0, size),
        (0, 0),
        (Drawing.STOP_DRAWING, 0)
    ]

    for x, y in points:
        serial.writePoint(x, y)

    print("Waiting...")
    time.sleep(6)
    serial.read()


if __name__ == '__main__':
    # testEnterExitDrawingMode()
    # testEnterExitManualControlMode()
    # testSetStepperDelay()
    # testSetDelayDrawManualControl()
    # testSetDelayManualControlMovement()

    testDrawTinySquare()
