import time

from base.db_service_base import DbServiceBase
from base.db_utils import db_utils_wrapper

BALANCE_FIELD_ID = "id"
BALANCE_FIELD_BTC = "btc"
BALANCE_FIELD_USD = "usd"
BALANCE_FIELD_CNY = "cny"


class BalanceService(DbServiceBase):
    def __init__(self, table_name="balance", table_date=None):
        super().__init__()
        self.select_info_str = "id, btc, usd, cny"
        if table_date is None:
            self.table_date = time.strftime('%Y%m%d', time.localtime(time.time()))
        else:
            self.table_date = table_date
        self.table_name = "{}_{}".format(table_name, self.table_date)

    def _get_select_info(self, row):
        return {
            BALANCE_FIELD_ID: int(row[0]),
            BALANCE_FIELD_BTC: float(row[1]),
            BALANCE_FIELD_USD: float(row[2]),
            BALANCE_FIELD_CNY: float(row[3])
        }

    """
    query methods
    """

    @db_utils_wrapper
    def get_all(self, cursor):
        """
        get all balance
        :param cursor: db cursor
        :return: balance list
        """
        sql_str = \
            "select {} from {}".format(
                self.select_info_str, self.table_name
            )
        return self._get_infos(cursor=cursor, sql_str=sql_str)

    @db_utils_wrapper
    def get_by_id(self, cursor, id):
        """
        get balance by id
        :param cursor: db cursor
        :param id:
        :return: a balance information
        """
        sql_str = \
            "select {} from {} where id={}".format(
                self.select_info_str, self.table_name, id
            )
        return self._get_item(cursor=cursor, sql_str=sql_str)

    """
    insert method
    """

    @db_utils_wrapper
    def insert_balance(self, cursor, balances, max_insert_once=8):
        """
        insert balance
        :param cursor: db cursor
        :param balance: balance information
        :return: inserted count when success, otherwise return None by db_utils_wrapper
        """
        if len(balances) == 0:
            return 0

        prefix_sql_str = \
            "insert into {} " \
            "(id, btc, usd, cny) values".format(self.table_name)
        sql_value_fmt = \
            "({}, {}, {}, {})"

        sql_str = prefix_sql_str
        cnt = 0
        inserted_cnt = 0
        for balance in balances:
            id = balance[BALANCE_FIELD_ID]
            btc = balance.get(BALANCE_FIELD_BTC, 0.0)
            usd = balance.get(BALANCE_FIELD_USD, 0.0)
            cny = balance.get(BALANCE_FIELD_CNY, 0.0)
            sql_str = sql_str + sql_value_fmt.format(id, btc, usd, cny)
            cnt += 1
            if cnt >= max_insert_once:
                cnt = 0
                inserted_cnt += self._insert_item_ret_cnt(cursor=cursor, sql_str=sql_str)
                sql_str = prefix_sql_str
            else:
                if balance != balances[-1]:
                    sql_str = sql_str + ","
        if cnt != 0:
            inserted_cnt += self._insert_item_ret_cnt(cursor=cursor, sql_str=sql_str)
        return inserted_cnt

    """
    delete method
    """

    @db_utils_wrapper
    def del_balance_by_id(self, cursor, id):
        """
        delete balance by id
        :param cursor: db cursor
        :param id: balance id
        :return: removed count when success, otherwise return None by db_utils_wrapper
        """
        sql_str = \
            "delete from {} where id={}".format(
                self.table_name, id
            )
        return self._del_items(cursor=cursor, sql_str=sql_str)
