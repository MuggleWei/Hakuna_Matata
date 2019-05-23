import logging.config
import os
import unittest

from service.balance_service import *
from service.daily_table_service import DailyTableService


class TestServices(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        super().setUpClass()
        daily_table_service = DailyTableService()

        ret = daily_table_service.create_daily_tables(
            template_suffix="template",
            need_copy_content=False,
            currdate="20190101"
        )

        balance_service = BalanceService(table_date="20190101")
        balances = []
        for i in range(18):
            balances.append({
                BALANCE_FIELD_ID: i + 1,
                BALANCE_FIELD_BTC: i,
                BALANCE_FIELD_USD: i * 10,
                BALANCE_FIELD_CNY: i * 100,
            })
        cnt = balance_service.insert_balance(balances=balances)
        logging.info("insert balance count {} into {}".format(cnt, balance_service.table_name))

    @classmethod
    def tearDownClass(cls):
        super().tearDownClass()

    def test_balance_service(self):
        daily_table_service = DailyTableService()
        daily_table_service.create_daily_tables(template_suffix="20190101", need_copy_content=True)

        balance_service = BalanceService()

        balances = balance_service.get_all()
        self.assertEqual(len(balances), 18)

        for i in range(18):
            balance = balance_service.get_by_id(id=i + 1)
            self.assertEqual(balance[BALANCE_FIELD_ID], i + 1)
            self.assertEqual(balance[BALANCE_FIELD_BTC], i)
            self.assertEqual(balance[BALANCE_FIELD_USD], i * 10)
            self.assertEqual(balance[BALANCE_FIELD_CNY], i * 100)

        for i in range(18):
            balance_service.del_balance_by_id(id=i + 1)
        balances = balance_service.get_all()
        self.assertEqual(len(balances), 0)


if __name__ == '__main__':
    if not os.path.exists("log"):
        os.makedirs("log")
    logging.config.fileConfig("../../config/log.conf")

    unittest.main()
