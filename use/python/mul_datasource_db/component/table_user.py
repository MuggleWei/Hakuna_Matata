import typing

from base.db_utils import DbUtils
from dao.dao_user import DaoUser


class TableUser(object):
    def __init__(self, table_name):
        """
        用户表
        :param table_name:
        """
        self._table_name = table_name

    def _get_dao(self, rows):
        """
        从行信息转化为对象信息
        :param rows:
        :return:
        """
        daos = []
        for row in rows:
            dao = DaoUser(
                id=int(row[0]),
                name=row[1],
                user_type=int(row[2]),
                status=int(row[3]))
            daos.append(dao)
        return daos

    def fetch_all(self, source) -> typing.List[DaoUser]:
        """
        获取所有用户信息
        :param source: 数据源名
        :return:
        """
        strsql = \
            "select id, name, user_type, status " \
            "from {}".format(self._table_name)
        db_utils = DbUtils()
        rows = db_utils.fetch_all(source=source, strsql=strsql)
        return self._get_dao(rows)

    def insert(self, source, daos: typing.List[DaoUser], batch_cnt=16):
        """
        插入用户
        :param source:
        :param daos:
        :param batch_cnt:
        :return:
        """
        rows = []
        for dao in daos:
            row = [dao.id, dao.name, dao.user_type, dao.status]
            rows.append(row)

        strsql = \
            "insert into {} " \
            "(id, name, user_type, status) " \
            "values (%s,%s,%s,%s) " \
            "ON DUPLICATE KEY UPDATE " \
            "name=VALUES(name), " \
            "user_type=VALUES(user_type), " \
            "status=VALUES(status)".format(self._table_name)
        db_utils = DbUtils()
        return db_utils.batch_insert(source=source, strsql=strsql, rows=rows, batch_cnt=batch_cnt)
