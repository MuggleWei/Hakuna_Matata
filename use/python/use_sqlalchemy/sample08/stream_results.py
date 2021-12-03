"""
流式处理
"""

import sqlalchemy

if __name__ == "__main__":
    engine = sqlalchemy.create_engine(
        "mysql+pymysql://muggle:wsz123@localhost/db_hello",
        echo=False, future=True)

    # prepare datas
    with engine.connect() as conn:
        conn.execute(sqlalchemy.text("DROP TABLE IF EXISTS t_hello"))
        conn.execute(sqlalchemy.text("CREATE TABLE IF NOT EXISTS t_hello (x int, y int)"))
        conn.commit()
        print("new table")

        values = []
        insert_cnt = 0
        for i in range(100000):
            insert_cnt += 1
            values.append({"x": i, "y": i * 2})
            if len(values) > 128:
                conn.execute(
                    sqlalchemy.text("INSERT INTO t_hello (x, y) VALUES (:x, :y)"),
                    values)
                conn.commit()
                values.clear()
                print("already insert row count: {}".format(insert_cnt))

        if len(values) > 0:
            conn.execute(
                sqlalchemy.text("INSERT INTO t_hello (x, y) VALUES (:x, :y)"),
                values)
            conn.commit()
            print("total insert row count: {}".format(insert_cnt))

    # stream fetch
    cnt = 0
    with engine.connect() as conn:
        conn = conn.execution_options(stream_results=True, max_row_buffer=100)
        result = conn.execute(sqlalchemy.text("select x, y from t_hello"))
        for row in result:
            cnt += 1
            if cnt % 1000 == 0:
                print("handle row count: {}".format(cnt))
                print("already read row count: {}".format(cnt))
        print("total read row count: {}".format(cnt))

    # drop table
    with engine.connect() as conn:
        conn.execute(sqlalchemy.text("DROP TABLE IF EXISTS t_hello"))
