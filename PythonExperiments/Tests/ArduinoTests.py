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
    time.sleep(2)  # Wait
    serial.read()

    # Switch out of drawing mode
    serial.writePoint(0xFF00, 0xFF00)
    print('waiting...')
    time.sleep(2)
    serial.read()

    print('waiting...')
    time.sleep(2)
    serial.read()

    serial._port.close()


if __name__ == '__main__':
    testEnterExitDrawingMode()
