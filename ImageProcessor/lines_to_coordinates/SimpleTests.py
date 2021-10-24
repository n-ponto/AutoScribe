import numpy as np
import unittest

from lines_to_coordinates import lines_to_coordinates

class SimpleTests(unittest.TestCase):

    def test_upper(self):
        self.assertEqual('foo'.upper(), 'FOO')

    def test_isupper(self):
        self.assertTrue('FOO'.isupper())
        self.assertFalse('Foo'.isupper())

    def test_split(self):
        s = 'hello world'
        self.assertEqual(s.split(), ['hello', 'world'])
        # check that s.split fails when the separator is not a string
        with self.assertRaises(TypeError):
            s.split(2)

    def test_find_start(self):
        SZ = 16
        lines: np.ndarray = np.zeros((SZ, SZ), dtype=np.uint8)
        start = (8, 9)
        lines[start] = 255
        expected: list = [start]
        actual: list = lines_to_coordinates(lines)
        self.assertEqual(expected, actual)

    def test_horizontal_line(self):
        # Input
        SZ = 16
        lines: np.ndarray = np.zeros((SZ, SZ), dtype=np.uint8)
        ROW = 8
        lines[ROW, :] = 255 # Horizontal line
        # Expected output
        expected: list = [(ROW, 0), (ROW, SZ)]

        # Execute
        actual: list = lines_to_coordinates(lines)

        # Verify
        self.assertEqual(expected, actual)

    def test_vertical_line(self):
        # Input
        SZ = 17
        lines: np.ndarray = np.zeros((SZ, SZ), dtype=np.uint8)
        COL = 12
        lines[:, COL] = 255 # Vertical line
        # Expected output
        expected: list = [(0, COL), (SZ, COL)]

        # Execute
        actual: list = lines_to_coordinates(lines)

        # Verify
        self.assertEqual(expected, actual)

if __name__ == '__main__':
    unittest.main()