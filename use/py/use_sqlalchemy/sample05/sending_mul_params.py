import sqlalchemy

if __name__ == "__main__":
    engine = sqlalchemy.create_engine(
        "mysql+pymysql://muggle:wsz123@localhost/db_hello",
        echo=False, future=True)

    with engine.connect() as conn:
        conn.execute(sqlalchemy.text("CREATE TABLE IF NOT EXISTS t_hello (x int, y int)"))
        conn.commit()

    with engine.connect() as conn:
        """
        这里一次性插入了多行数据, 在这背后, 使用的是
        DBAPI的被成为cursor.executemany()的特性
        """
        conn.execute(
            sqlalchemy.text("INSERT INTO t_hello (x, y) VALUES (:x, :y)"),
            [{"x": 11, "y": 12}, {"x": 13, "y": 14}]
        )
        conn.commit()
