import logging

from component import db_utils


class DbServiceBase:
    def _get_select_info(self, row):
        """
        从cursor读取的行，到数据的转换函数
        :param row: cursor读取的行
        :return: 转换得到的数据
        """
        return None

    def _get_infos(self, cursor, sql_str, get_select_info=None):
        """
        获取信息，结果为list
        :param cursor: cursor
        :param sql_str: 执行的sql语句
        :return: 查询得到的结果list
        """
        logging.debug(sql_str)
        cursor.execute(sql_str)

        result = []
        rows = cursor.fetchall()
        for row in rows:
            if get_select_info is None:
                item = self._get_select_info(row)
            else:
                item = get_select_info(row)
            result.append(item)

        logging.debug(result)
        return result

    def _get_item(self, cursor, sql_str, get_select_info=None):
        """
        获取信息，结果为对象
        :param cursor:
        :param sql_str: 执行的sql语句
        :return: 查询得到的单个对象
        """
        logging.debug(sql_str)
        cursor.execute(sql_str)

        result = []
        rows = cursor.fetchall()
        if len(rows) != 1:
            logging.debug("None")
            return None

        if get_select_info is None:
            item = self._get_select_info(rows[0])
        else:
            item = get_select_info(rows[0])
        logging.debug(item)
        return item

    def _del_items(self, cursor, sql_str):
        logging.debug(sql_str)
        cursor.execute(sql_str)

        cnt = db_utils.get_cursor_rowcnt(cursor=cursor)
        logging.debug("delete row: {}".format(cnt))
        return cnt

    def _insert_item_ret_idx(self, cursor, sql_str):
        logging.debug(sql_str)
        cursor.execute(sql_str)

        id = cursor.lastrowid
        logging.debug("inserted get increment id: {}".format(id))
        return id

    def _insert_item_ret_cnt(self, cursor, sql_str):
        logging.debug(sql_str)
        cursor.execute(sql_str)

        cnt = db_utils.get_cursor_rowcnt(cursor=cursor)
        return cnt

    def _update_item_ret_cnt(self, cursor, sql_str):
        logging.debug(sql_str)
        cursor.execute(sql_str)

        cnt = db_utils.get_cursor_rowcnt(cursor=cursor)
        return cnt
