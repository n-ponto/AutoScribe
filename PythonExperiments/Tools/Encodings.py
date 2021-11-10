'''
Definitions for encoding messages when sending over the serial port
Ensure that these encodings match with what the Ardino code expects to receive. 
'''

# Defined in Commands.h
class Commands:
    SET_PEN_RANGE =         0
    CHANGE_PEN_ANGLE =      1
    MOVE_TO_COORDINATE =    2
    RESET_HOME =            3
    SET_STEPPER_DELAY =     4
    ENTER_DRAW_MODE =       8
    ENTER_MANUAL_STEP_MODE= 9
    
