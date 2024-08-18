import os
import pickle

DATA_FILE_NAME = "settings.pkl"
DATA_FILE_PATH = os.path.join(os.path.dirname(
    os.path.realpath(__file__)), DATA_FILE_NAME)


class Defaults:
    ''' These defaults should correspond to the defaults from Hardware.h file'''
    PenUpHeight = 50
    PenDownHeight = 70
    DrawSpeed = 250


class DataObject:

    # The directory
    WorkingDirectory: str
    PenUpHeight: int
    PenDownHeight: int
    MoveSpeed: int

    def __init__(self):
        default_wd = os.path.realpath(__file__)
        default_wd = os.path.realpath(os.path.join(default_wd, '../images'))
        self.WorkingDirectory = default_wd
        self.PenUpHeight = Defaults.PenUpHeight
        self.PenDownHeight = Defaults.PenDownHeight
        self.MoveSpeed = Defaults.DrawSpeed

    def initDefault(self) -> None:
        '''Default values should be set if no settings file exists.'''

    def saveData(self):
        '''Saves the current data object to the settings file.'''
        with open(DATA_FILE_PATH, 'wb') as f:
            pickle.dump(self, f)

    @staticmethod
    def tryLoadData():
        ''' Try to load the data from the settings file, creates new data object
        with defaults if the file does not exist.'''
        if not os.path.exists(DATA_FILE_PATH):
            return DataObject()
        else:
            print(f'Loading data from {DATA_FILE_PATH}')
            with open(DATA_FILE_PATH, 'rb') as f:
                return pickle.load(f)
