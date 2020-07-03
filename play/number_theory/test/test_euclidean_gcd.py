import math
import unittest

from utils import euclidean_gcd


class TestEuclideanGcd(unittest.TestCase):
    def test_gcd(self):
        for a in range(-999, 1000, 1):
            for b in range(a, 1000, 1):
                g1 = math.gcd(a, b)
                g2 = euclidean_gcd(a, b)
                if a > 0 and b > 0:
                    self.assertEqual(g1, g2)
                else:
                    self.assertEqual(abs(g1), abs(g2))


if __name__ == '__main__':
    unittest.main()
