"""
提交变更
"""

import sqlalchemy

if __name__ == "__main__":
    engine = sqlalchemy.create_engine(
        "mysql+pymysql://muggle:wsz123@localhost/db_hello",
        echo=False, future=True)

    with engine.connect() as conn:
        conn.execute(sqlalchemy.text("DROP TABLE IF EXISTS t_hello"))
        conn.commit()

    with engine.connect() as conn:
        """
        这里相当于执行了一个事务, 需要记得手动commit
        在commit之后, 还可以继续执行语句, 接着再commit
        BEGIN(implicit);
        CRATE TABLE t_hello (x int, y int);
        INSERT INTO t_hello (x, y) values ((1,1),(2,4));
        COMMIT;
        """
        conn.execute(sqlalchemy.text("CREATE TABLE t_hello (x int, y int)"))
        conn.execute(
            sqlalchemy.text("INSERT INTO t_hello (x, y) VALUES (:x, :y)"),
            [{"x": 1, "y": 1}, {"x": 2, "y": 4}]
        )

        # # 这里故意造成一个错误, 可以看看表是否存在
        # conn.execute(
        #     sqlalchemy.text("INSERT INTO t_hello (x, y) VALUES (:x, :y)"),
        #     [{"z": 1, "y": 1}]
        # )

        conn.commit()

    with engine.begin() as conn:
        """
        这里也相当于执行了事务, 不同的是不需要手动commit, 在代码块结束时会自动提交
        """
        conn.execute(
            sqlalchemy.text("INSERT INTO t_hello (x, y) VALUES (:x, :y)"),
            [{"x": 5, "y": 5}]
        )

    # 想要使用 手动提交(commit as you go) 或者 块结束自动提交(begin once),
    # 完全取决于场景和个人喜好
