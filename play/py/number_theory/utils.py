from sympy import symbols, Poly


def is_square(n):
    """
    判断n是否是平方数
    :param n:
    :return: True或False
    """
    low = 1
    high = n
    while low <= high:
        mid = int((low + high) / 2)
        power = mid * mid
        if power > n:
            high = mid - 1
        elif power < n:
            low = mid + 1
        else:
            return True
    return False


def euclidean_gcd(a, b):
    """
    使用欧几里得算法, 计算两个数的最大公约数
    :param a:
    :param b:
    :return: gcd(a,b)
    """
    if a == 0 and b == 0:
        return 0
    elif a == 0 and b != 0:
        return b
    elif a != 0 and b == 0:
        return a

    while b != 0:
        # 不使用 % , 因为 % 在不同的编程语言中的结果可能是不同的(虽然都是同余的)
        # a, b = b, a % b
        q = int(a / b)
        r = a - q * b
        a, b = b, r
    return a


def euclidean_linear_combination(a, b, show_trace=False):
    """
    求线性方程的 ax + by = gcd(a,b) 的一个解 (x1, y1)
    根据线性方程定理, 方程的一般解可由 (x1 + k * (b / g), y1 - k * (a / g)) 得到
    其中 g = gcd(a,b), k为任意整数
    :param a:
    :param b:
    :param show_trace: 显示计算过程
    :return: 返回 x1, y1, gcd(a,b)
    """
    if a == 0 and b == 0:
        return 0, 0, 0
    elif a == 0 and b != 0:
        return 0, 1, b
    elif a != 0 and b == 0:
        return 1, 0, a

    # 使用欧几里得算法求最大公约数并记录中间过程的除数与余数
    q_list = []
    r_list = [a, b]
    while b != 0:
        q = int(a / b)
        r = a - q * b
        q_list.append(q)
        a, b = b, r
        r_list.append(r)
    g = a

    # 递推获取得到解的方程
    a, b = symbols('a b')
    eq_list = []
    eq_list.append(a)
    eq_list.append(b)
    len_list = len(q_list) - 1
    for i in range(len_list):
        eq_list.append(eq_list[i] - q_list[i] * eq_list[i + 1])
        if show_trace is True:
            print("{} = {} * {} + {} => {} = {}".format(
                r_list[i], q_list[i], r_list[i + 1], r_list[i + 2], r_list[i + 2], eq_list[-1]))

    # 获取方程中a, b的系数
    p = Poly(eq_list[-1])
    x1 = None
    y1 = None
    for monom, coeff in p.as_dict().items():
        if monom[0] == 1:
            x1 = coeff
        elif monom[1] == 1:
            y1 = coeff
        else:
            continue
    return x1, y1, g


def linear_congruence(a, c, m):
    """
    求线性同余方程 ax=c(mod m) 的解
    :param a:
    :param c:
    :param m:
    :return: 解的列表
    """
    if a == 0 or m == 0:
        raise Exception("linear congruence input invalid arguments")

    u, v, g = euclidean_linear_combination(a, m)
    if int(c / g) * g != c:
        return []

    sol_list = []
    x0 = int(u * c / g)
    for k in range(0, g):
        sol_list.append(x0 + int(k * m / g))
    return sol_list
