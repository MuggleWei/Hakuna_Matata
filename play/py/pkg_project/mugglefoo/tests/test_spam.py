import unittest
from src.mugglefoo.utils.spam import Spam


class TestSpam(unittest.TestCase):
    def setUp(self):
        self._spam = Spam()

    def test_get_ip(self):
        ip = self._spam.get_ip()
        if ip is not None:
            self.assertTrue(isinstance(ip, str))


if __name__ == "__main__":
    unittest.main()
