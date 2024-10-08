'''
Definitions for encoding messages when sending over the serial port
Ensure that these encodings match with what the Ardino code expects to receive. 
'''
# Proprietary string encoding to convey the move command
NCODE_MOVE = 'MOVE'


class Commands:
    '''Commands that can be sent to the Arduino. Defined in Commands.h'''
    SET_PEN_RANGE = 1
    CHANGE_PEN_ANGLE = 2
    MOVE_TO_COORDINATE = 0  # Not used
    RESET_HOME = 0  # Not used
    SET_MOVE_SPEED = 3
    DISCONNECT = 4
    ENTER_DRAW_MODE = 8
    ENTER_MANUAL_CONTROL_MODE = 9

# Special encodings for the drawing runtime mode
# Make sure these constants line up with the values from
# the Arduino code header files


class Drawing:
    BUFSZ = 0x100
    EMERGENCY_STOP = 0x1FFF
    MOVE_PEN = (1 << 14)  # Should move the pen (down if PEN_UP == 0)
    STOP_DRAWING = (1 << 15)
    FLAG_MASK = (0b11 << 14)

# Special encodings for the manual control runtime mode


class MCKeys:
    STOP = 0
    SP_P = 1
    SP_R = 2
    UP_P = 3
    UP_R = 4
    DN_P = 5
    DN_R = 6
    LT_P = 7
    LT_R = 8
    RT_P = 9
    RT_R = 10


class ConsoleColors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'
