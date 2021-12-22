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
    ENTER_MANUAL_CONTROL_MODE= 9

# Special encodings for the drawing runtime mode
class Drawing:
    EMERGENCY_STOP = 0x7FFF  
    MOVE_PEN =       (1 << 12)  # Should move the pen (down if PEN_UP == 0)
    PEN_UP =         (1 << 13)  # Move the pen up or down
    STOP_DRAWING =   (1 << 14)

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
    
