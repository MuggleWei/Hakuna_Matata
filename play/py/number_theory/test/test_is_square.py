import unittest

from utils import is_square


class TestIsSquare(unittest.TestCase):
    def test_is_square(self):
        self.assertTrue(is_square(1))
        self.assertFalse(is_square(2))
        self.assertFalse(is_square(3))
        self.assertTrue(is_square(4))
        self.assertFalse(is_square(6))
        self.assertFalse(is_square(7))
        self.assertFalse(is_square(8))
        self.assertTrue(is_square(9))
        self.assertTrue(is_square(81))


if __name__ == '__main__':
    unittest.main()
