import os
import pickle

DATA_FILE_NAME = "settings.pkl"
DATA_FILE_PATH =  os.path.join(os.path.dirname(os.path.realpath(__file__)), DATA_FILE_NAME)
class DataObject:

    # The directory 
    WorkingDirectory: str
    PenUpHeight: int
    PenDownHeight: int

    def __init__(self, default=False):
        if default: self.initDefault()    

    def initDefault(self) -> None:
        ''' Default values should be set if no settings file exists
        '''
        self.WorkingDirectory = "C:/Users/noahp/Documents/Projects/AutoScribe/images"
        self.PenUpHeight = 50
        self.PenDownHeight = 70


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
