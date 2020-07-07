import unittest

from utils import linear_congruence


class TestLinearCongruence(unittest.TestCase):
    def linear_cong_result(self, a, c, m, result_len):
        sol_list = linear_congruence(a, c, m)
        self.assertEqual(len(sol_list), result_len)
        other_sol = []
        for s in sol_list:
            self.assertEqual((s * a - c) % m, 0)
            for other_s in other_sol:
                self.assertNotEqual((s - other_s) % m, 0)
            other_sol.append(s)

    def test_linear_cong(self):
        self.linear_cong_result(a=18, c=8, m=22, result_len=2)
        self.linear_cong_result(a=943, c=381, m=2576, result_len=0)
        self.linear_cong_result(a=893, c=266, m=2432, result_len=19)


if __name__ == '__main__':
    unittest.main()
