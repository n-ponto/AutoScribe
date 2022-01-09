import time
import sys
import os
import tkinter as tk
from tkinter import filedialog as fd
# Import Serial Port and Encodings
sys.path.append(os.path.dirname(__file__) + "/../..")
from Tools.SerialPort import SerialPort
from Tools.NcodeSender import NcodeSender
from Tools.Encodings import *

class DrawingFrame(tk.Frame):
    '''
    Allows user to select the ncode file to send and trasmits the data
    '''
    _serial: SerialPort = None
    _sender: NcodeSender = None
    _filename: str = None
    _lbl_file: tk.Label
    _btn_send: tk.Button
    

    def __init__(self, master: tk.Misc, sp: SerialPort):
        super().__init__(master=master)
        self._serial = sp
        self._sender = NcodeSender(sp)
        self._render()

    def _render(self):
        btn_file = tk.Button(self, text="Select File", command=self._selectFile)
        btn_file.pack(padx=5, pady=5)

        self._lbl_file = tk.Label(self, text="Please select a file.")
        self._lbl_file.pack(padx=5, pady=5)

        self._btn_send = tk.Button(self, text="Send Ncode", command=self._send, state='disabled')
        self._btn_send.pack(padx=5, pady=5)

    def _selectFile(self):
        filetypes = (
            ('ncode files', '*.ncode'),
            ('All files', '*.*')
        )

        path = fd.askopenfilename(
            title='Open a file',
            initialdir='./',
            filetypes=filetypes)

        #TODO: check that the file is valid?
        if path is None or len(path) < 6:
            print("File dialog closed")
            return
        self._filename = path
        self._lbl_file.config(text=self._filename)
        self._btn_send['state'] = 'normal'

    def _send(self):
        #TODO: check file is valid again?
        print(f"Sending file {self._filename}")
        self._sender.send(self._filename)


if __name__ == "__main__":
    root = tk.Tk()  # Create root window
    root.title("Drawing Frame")  # Set window title
    root.geometry("500x500")
    sp = SerialPort()  # Initialize serial port
    drawingFrame = DrawingFrame(root, sp)  # Create frame
    drawingFrame.pack()  # Add the frame to the root window
    sp.awaitResponse()

    root.mainloop()
    sp.read()