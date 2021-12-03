"""
使用ORM session
"""

import sqlalchemy

from sqlalchemy.orm import Session

if __name__ == "__main__":
    engine = sqlalchemy.create_engine(
        "mysql+pymysql://muggle:wsz123@localhost/db_hello",
        echo=False, future=True)

    with engine.connect() as conn:
        conn.execute(sqlalchemy.text("CREATE TABLE IF NOT EXISTS t_hello (x int, y int)"))
        conn.commit()

    stmt = sqlalchemy.text(
        "SELECT x, y FROM t_hello WHERE y > :y ORDER BY x, y").bindparams(y=6)
    with Session(engine) as session:
        result = session.execute(stmt)
        for row in result:
            print("x: {}, y: {}".format(row.x, row.y))

        # 和Connection一样, Session也使用"commit as you go"的特性
        result = session.execute(
            sqlalchemy.text("UPDATE t_hello SET y=:y WHERE x=:x"),
            [{"x": 9, "y": 11}, {"x": 13, "y": 15}]
        )
        session.commit()
