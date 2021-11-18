import typing

from base.db_utils import DbUtils
from dao.dao_fund_stream_record import DaoFundStreamRecord


class TableFundStreamRecord(object):
    def __init__(self, table_name):
        """
        资金流水表
        :param table_name:
        """
        self._table_name = table_name

    def get_head(self, source):
        """
        获取表头
        :param source: 数据源
        """
        strsql = "desc {}".format(self._table_name)
        db_utils = DbUtils()
        head_infos = db_utils.fetch_all(source=source, strsql=strsql)
        head = []
        for v in head_infos:
            head.append(v[0])
        return head

    def stream_fetch(self, source, callback, user_id, *args, **kwargs):
        """
        流式获取资金流水数据
        :param source: 数据源
        :param callback: 回调函数
        """
        strsql = \
            "select id, user_id, dir, amount " \
            "from {} " \
            "where user_id={}".format(self._table_name, user_id)
        db_utils = DbUtils()
        return db_utils.stream_execute(
            source, strsql, callback,
            *args, **kwargs)

    def insert(self, source, records: typing.List[DaoFundStreamRecord], batch_cnt=32):
        """
        插入资金流水记录
        :param source: 数据源
        :param records: 资金流水记录
        :param batch_cnt: 单次批量插入数量
        :return:
        """
        rows = []
        for dao in records:
            row = [dao.id, dao.user_id, dao.dir, dao.amount]
            rows.append(row)

        strsql = \
            "insert into {} " \
            "(id, user_id, dir, amount) " \
            "values (%s,%s,%s,%s) " \
            "ON DUPLICATE KEY UPDATE " \
            "user_id=VALUES(user_id), " \
            "dir=VALUES(dir), " \
            "amount=VALUES(amount)".format(self._table_name)
        db_utils = DbUtils()
        return db_utils.batch_insert(source=source, strsql=strsql, rows=rows, batch_cnt=batch_cnt)
