"""
获取行
"""

import sqlalchemy

if __name__ == "__main__":
    engine = sqlalchemy.create_engine(
        "mysql+pymysql://muggle:wsz123@localhost/db_hello",
        echo=False, future=True)

    with engine.connect() as conn:
        conn.execute(sqlalchemy.text("CREATE TABLE IF NOT EXISTS t_hello (x int, y int)"))
        conn.commit()

    with engine.connect() as conn:
        result = conn.execute(sqlalchemy.text("SELECT x, y FROM t_hello"))
        for row in result:
            # 可直接通过字段访问
            print("x: {}, y: {}".format(row.x, row.y))

            # 也可以通过列下标访问
            print("row[0]: {}, row[1]: {}".format(row[0], row[1]))

    with engine.connect() as conn:
        result = conn.execute(sqlalchemy.text("SELECT x, y FROM t_hello"))
        for dict_row in result.mappings():
            # 也可以使用mappings, 作为字典访问
            print("dict_row['x']: {}, dict_row['y']: {}".format(dict_row['x'], dict_row['y']))
