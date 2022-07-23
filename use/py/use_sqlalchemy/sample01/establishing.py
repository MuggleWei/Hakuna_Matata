"""
建立连接
"""

import sqlalchemy

if __name__ == "__main__":
    engine = sqlalchemy.create_engine(
        "mysql+pymysql://muggle:wsz123@localhost/db_hello",
        echo=False, future=True)

    with engine.connect() as conn:
        result = conn.execute(sqlalchemy.text("select 'hello world'"))
        print(result.all())
