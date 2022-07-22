"""
∀a, b, 观察线性方程 a*x + b*y 的结果
"""
import os

if __name__ == '__main__':
    a = 42
    b = 30
    start = -50
    end = 50 + 1
    range_val = [v for v in range(start, end, 1)]

    # 生成表
    table = []
    for y in range_val:
        row = []
        for x in range_val:
            row.append(a * x + b * y)
        table.append(row)

    # 写入文件
    if not os.path.exists('log'):
        os.mkdir('log')
    with open('log/linear_comb_table.csv', 'w') as f:
        f.write('a={} and b={},'.format(a, b))
        for x in range_val:
            f.write("{},".format(x))
        f.write("\n")

        i = 0
        for row in table:
            f.write("{},".format(range_val[i]))
            i += 1
            for col in row:
                f.write("{},".format(col))
            f.write("\n")
