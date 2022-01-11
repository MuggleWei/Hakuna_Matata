import math


class Bar(object):
    def length(self, a, b):
        return math.sqrt(a**2 + b**2)

    def fun(self, n):
        if n == 1:
            return -1
        elif n == 2:
            return -2
        else:
            return n
