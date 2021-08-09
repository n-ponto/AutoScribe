class Default():
    UpAngle: int = 80
    DownAngle: int = 100
    Speed: int = 700

class Commands():
    SET_PEN_RANGE = 0
    CHANGE_PEN_ANGLE = 1
    MOVE_TO_COORDINATE = 2
    RESET_HOME = 3
    DRAW = 4
    MANUAL_STEP = 5

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