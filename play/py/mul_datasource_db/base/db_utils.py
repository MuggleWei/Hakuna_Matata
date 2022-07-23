import logging

import pymysql

from base.data_source import DataSource
from base.data_source_config import DataSourceConfig
from base.singleton import singleton


@singleton
class DbUtils:
    class DbUtilsConn:
        def __init__(self, source, fn_get, fn_recycle):
            self._source = source
            self._conn = fn_get(source=source)
            self._recycle = fn_recycle

        def __enter__(self):
            return self._conn

        def __exit__(self, exc_type, exc_val, exc_tb):
            self._recycle(self._conn)

    def fetch_conn(self, source):
        """
        获取source的连接, 支持with语法
        :param source: 数据源名
        :return:
        """
        dbconn = self.DbUtilsConn(
            source=source,
            fn_get=self._get_conn,
            fn_recycle=self._recycle_conn)
        return dbconn

    def stream_execute(self, source, strsql, callback, *args, **kwargs):
        """
        流式执行

        注意: 有的版本流式游标复用连接会出现问题, 所以这里每次使用流式读取都
              使用new_conn新建一个连接, 而不是使用fetch_conn/get_conn获取连接

        :param source: 数据源名
        :param strsql: sql语句
        :param callback: 回调函数
        :param args: 回调函数入参
        :param kwargs: 回调函数入参
        :return: 是否全部成功
        """
        cnt = 0
        logging.debug("DbUtils stream_execute, source: {}, do sql: {}".format(source, strsql))
        try:
            with self._new_conn(source) as conn:
                with self._get_stream_cursor(conn) as cursor:
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
        logging.debug("complete sql: {}, effect row: {}".format(strsql, cnt))
        return True

    def stream_dict_execute(self, source, strsql, callback, *args, **kwargs):
        """
        流式字典执行

        注意: 有的版本流式游标复用连接会出现问题, 所以这里每次使用流式读取都
              使用new_conn新建一个连接, 而不是使用fetch_conn/get_conn获取连接

        :param source: 数据源名
        :param strsql: sql语句
        :param callback: 回调函数
        :param args: 回调函数入参
        :param kwargs: 回调函数入参
        :return: 是否全部成功
        """
        cnt = 0
        logging.debug("DbUtils stream_dict_execute, source: {}, do sql: {}".format(source, strsql))
        try:
            with self._new_conn(source) as conn:
                with self._get_stream_dict_cursor(conn) as cursor:
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
        logging.debug("complete sql: {}, effect row: {}".format(strsql, cnt))
        return True

    def fetch_all(self, source, strsql):
        """
        一次性读取所有数据
        :param source: 数据源名
        :param strsql:sql语句
        :return: 返回所有数据
        """
        logging.debug("DbUtils fetch_all, source: {}, do sql: {}".format(source, strsql))
        with self.fetch_conn(source) as conn:
            with self._get_cursor(conn) as cursor:
                cursor.execute(strsql)
                rows = cursor.fetchall()
        logging.debug("complete DbUtils fetch_all, source: {}, do sql: {}".format(source, strsql))
        return rows

    def fetch_all_dict(self, source, strsql):
        """
        一次性读取所有字典数据
        :param source: 数据源名
        :param strsql:sql语句
        :return: 返回所有数据
        """
        logging.debug("DbUtils fetch_all_dict, source: {}, do sql: {}".format(source, strsql))
        with self.fetch_conn(source) as conn:
            with self._get_dict_cursor(conn) as cursor:
                cursor.execute(strsql)
                rows = cursor.fetchall()
        logging.debug("complete DbUtils fetch_all_dict, source: {}, do sql: {}".format(source, strsql))
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
        logging.debug("DbUtils batch_insert, source: {}, do sql: {}".format(source, strsql))
        insert_cnt = 0
        with self.fetch_conn(source) as conn:
            with self._get_cursor(conn) as cursor:
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
        logging.debug("complete DbUtils batch_insert, affect row: {}, source: {}, do sql: {}".format(
            insert_cnt, source, strsql))
        return insert_cnt

    def update(self, source, strsql):
        """
        更新
        """
        logging.debug("DbUtils update, source: {}, do sql: {}".format(source, strsql))
        affect_row = 0
        with self.fetch_conn(source) as conn:
            with self._get_cursor(conn) as cursor:
                affect_row = cursor.execute(strsql)
                conn.commit()
        logging.debug("complete DbUtils update, affect row: {}, source: {}, do sql: {}".format(
            affect_row, source, strsql))
        return affect_row

    def _get_conn(self, source):
        """
        获取source的连接
        :param source: 数据源名
        :return:
        """
        # TODO: 使用连接池
        return self._new_conn(source)

    def _recycle_conn(self, conn):
        """
        回收连接
        :param conn:
        :return:
        """
        # TODO: 使用连接池
        conn.close()

    def _new_conn(self, source):
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

    def _get_cursor(self, conn):
        """
        获取游标
        :param conn:
        :return:
        """
        return conn.cursor()

    def _get_dict_cursor(self, conn):
        """
        获取字典游标
        :param conn: 数据库连接
        """
        return conn.cursor(cursor=pymysql.cursors.DictCursor)

    def _get_stream_cursor(self, conn):
        """
        获取流式游标
        :param conn: 数据库连接
        :return:
        """
        return conn.cursor(cursor=pymysql.cursors.SSCursor)

    def _get_stream_dict_cursor(self, conn):
        """
        获取流式字典邮编
        :param conn: 数据库连接
        """
        return conn.cursor(cursor=pymysql.cursors.SSDictCursor)
