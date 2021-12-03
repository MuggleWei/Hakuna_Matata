import sqlalchemy

if __name__ == "__main__":
    engine = sqlalchemy.create_engine(
        "mysql+pymysql://muggle:wsz123@localhost/db_hello",
        echo=False, future=True)

    with engine.connect() as conn:
        conn.execute(sqlalchemy.text("CREATE TABLE IF NOT EXISTS t_hello (x int, y int)"))
        conn.commit()

    with engine.connect() as conn:
        result = conn.execute(
            sqlalchemy.text("SELECT x, y FROM t_hello WHERE y > :y"),
            {"y": 2}
        )
        for row in result:
            print("x: {}, y: {}".format(row.x, row.y))
