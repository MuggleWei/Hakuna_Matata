"""
∀p∈素数集合, a∈N, 观察 a**k (mod p) 的结果
"""
import os

if __name__ == '__main__':
    p_list = [3, 5, 7, 11, 13, 17]

    # 生成表
    tables = {}
    for p in p_list:
        a_list = [x for x in range(1, p*2+1)]
        k_list = [x for x in range(1, p*2+1)]
        table = []
        for a in a_list:
            row = []
            for k in k_list:
                row.append((a ** k) % p)
            table.append(row)
        tables[p] = table

    # 写入文件
    if not os.path.exists('log'):
        os.mkdir('log')
    for p, table in tables.items():
        a_list = [x for x in range(1, p * 2 + 1)]
        k_list = [x for x in range(1, p * 2 + 1)]
        with open('log/a_power_k_mod_{}.csv'.format(p), 'w') as f:
            f.write('p={}|a^k(mod p),'.format(p))
            for k in k_list:
                f.write('a^{},'.format(k))
            f.write('\n')
            for i in range(len(table)):
                f.write('a={},'.format(a_list[i]))
                row = table[i]
                for col in row:
                    f.write('{},'.format(col))
                f.write('\n')
