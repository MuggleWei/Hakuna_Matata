import logging

import pymysql

from base.data_source import DataSource
from base.data_source_config import DataSourceConfig
from base.singleton import singleton


@singleton
class DbUtils:
    def get_conn(self, source):
        """
        获取source的连接
        :param source: 数据源名
        :return:
        """
        # TODO: 使用连接池
        return self.new_conn(source)

    def recycle_conn(self, conn):
        """
        回收连接
        :param conn:
        :return:
        """
        # TODO: 使用连接池
        conn.close()

    def new_conn(self, source):
        """
        新建一个连接
        :param source: 数据源名
        :return:
        """
        ds_cfg = DataSourceConfig()
        ds_item: DataSource = ds_cfg.get(source)
        if ds_item is None:
            logging.error("failed get db source by name: {}".format(source))
            return None

        conn = pymysql.connect(
            host=ds_item.ip,
            port=ds_item.port,
            user=ds_item.user,
            passwd=ds_item.passwd,
            db=ds_item.db,
            charset=ds_item.charset)
        return conn

    def get_cursor(self, conn):
        """
        获取游标
        :param conn:
        :return:
        """
        return conn.cursor()

    def get_stream_cursor(self, conn):
        """
        获取流式游标
        :param conn: 数据库连接
        :return:
        """
        return conn.cursor(cursor=pymysql.cursors.SSCursor)

    def stream_execute(self, source, strsql, callback, *args, **kwargs):
        """
        流式执行

        注意: 有的版本流式游标复用连接会出现问题, 所以这里每次使用流式读取都
              使用new_conn新建一个连接, 而不是使用get_conn获取连接

        :param source: 数据源名
        :param strsql: sql语句
        :param callback: 回调函数
        :param args: 回调函数入参
        :param kwargs: 回调函数入参
        :return: 是否全部成功
        """
        cnt = 0
        logging.info("DbUtils stream_execute, source: {}, do sql: {}".format(source, strsql))
        try:
            with self.new_conn(source) as conn:
                with self.get_stream_cursor(conn) as cursor:
                    cursor.execute(strsql)
                    while True:
                        row = cursor.fetchone()
                        if row is None:
                            break
                        callback(row, *args, **kwargs)
                        cnt += 1
        except Exception as e:
            logging.error("except do sql: {}, raise {}".format(strsql, e))
            return False
        logging.info("complete sql: {}, effect row: {}".format(strsql, cnt))
        return True

    def fetch_all(self, source, strsql):
        """
        一次性读取所有数据
        :param source: 数据源名
        :param strsql:sql语句
        :return: 返回所有数据
        """
        conn = self.get_conn(source)
        with self.get_cursor(conn) as cursor:
            cursor.execute(strsql)
            rows = cursor.fetchall()
        self.recycle_conn(conn)
        return rows

    def batch_insert(self, source, strsql, rows, batch_cnt):
        """
        批量插入信息
        :param source: 数据源名
        :param strsql: sql语句
        :param rows: 所有行信息
        :param batch_cnt: 一次batch多少行
        :return: 插入的数量
        """
        insert_cnt = 0
        conn = self.get_conn(source)
        with self.get_cursor(conn) as cursor:
            vals = []
            for row in rows:
                vals.append(row)
                if len(vals) >= batch_cnt:
                    insert_cnt += cursor.executemany(strsql, vals)
                    conn.commit()
                    vals.clear()
            if len(vals) > 0:
                insert_cnt += cursor.executemany(strsql, vals)
                conn.commit()
        self.recycle_conn(conn)
        return insert_cnt
