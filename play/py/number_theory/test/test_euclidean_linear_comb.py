import unittest

from utils import euclidean_linear_combination


class TestEuclideanLinearComb(unittest.TestCase):
    def print_linear_comb(self, a, b, show_trace=False):
        print("")
        print("a={0}, b={1}, slove gcd(a,b)={0}*x+{1}*y".format(a, b))
        x1, y1, g = euclidean_linear_combination(a, b, show_trace)
        print("result: {}*a + {}*b = gcd(a,b) = {}".format(x1, y1, g))
        result = x1 * a + y1 * b
        self.assertEqual(int(result), g)

        # 一般解: (x1 + k*b/g, y1 - k*a/g)
        print("general solution is ({} + k*{}, {} - k*{}), k∈Z".format(x1, int(b / g), y1, int(a / g)))

    def test_linear_comb(self):
        self.print_linear_comb(12345, 67890)
        self.print_linear_comb(12453, 2347)
        self.print_linear_comb(23198723169, 5468164814987)
        self.print_linear_comb(54321, 9876, show_trace=True)
        self.print_linear_comb(105, 121)
        self.print_linear_comb(12345, 34560, show_trace=True)
        self.print_linear_comb(-12345, 34560, show_trace=True)
        self.print_linear_comb(12345, -34560, show_trace=True)
        self.print_linear_comb(-12345, -34560, show_trace=True)


if __name__ == '__main__':
    unittest.main()
