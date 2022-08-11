import os
import time
import pickle
from Tools.Encodings import Commands

DATA_FILE_NAME = "settings.pkl"
DATA_FILE_PATH = os.path.join(os.path.dirname(
    os.path.realpath(__file__)), DATA_FILE_NAME)


class Defaults:
    ''' These defaults should correspond to the defaults from Hardware.h file
    '''
    PenUpHeight = 50
    PenDownHeight = 70


class DataObject:

    # The directory
    WorkingDirectory: str
    PenUpHeight: int
    PenDownHeight: int

    def __init__(self, default):
        if default:
            self.initDefault()

    def initDefault(self) -> None:
        ''' Default values should be set if no settings file exists
        '''
        default_wd = os.path.realpath(__file__)
        default_wd = os.path.realpath(os.path.join(default_wd, '../images'))
        self.WorkingDirectory = default_wd
        self.PenUpHeight = Defaults.PenUpHeight
        self.PenDownHeight = Defaults.PenDownHeight

    def initSettingsOnArduino(self, serialPort) -> None:
        if (self.PenUpHeight != Defaults.PenUpHeight or \
            self.PenDownHeight != Defaults.PenDownHeight):
            # Set pen range
            serialPort.writeByte(Commands.SET_PEN_RANGE)
            serialPort.writeByte(int(self.PenUpHeight))
            serialPort.writeByte(int(self.PenDownHeight))
            time.sleep(0.1)
            print("Loaded saved pen range settings to arduino")
            serialPort.readStr()


def tryLoadData() -> DataObject:
    ''' Try to load settings object from file, or create a new one 
    '''
    if not os.path.exists(DATA_FILE_PATH):
        return DataObject(default=True)
    else:
        print(f'Loading data from {DATA_FILE_PATH}')
        with open(DATA_FILE_PATH, 'rb') as f:
            return pickle.load(f)


def saveData(obj: DataObject):
    ''' Save the settings 
    '''
    with open(DATA_FILE_PATH, 'wb') as f:
        pickle.dump(obj, f)
