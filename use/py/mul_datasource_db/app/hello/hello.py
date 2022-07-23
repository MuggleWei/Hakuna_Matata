import getopt
import logging
import os
import sys

from app.hello import __version__
from app.hello.hello_cfg import HelloConfig
from base.data_source_config import DataSourceConfig
from base.log_handle import LogHandle
from component.table_user import TableUser
from component.table_fund_stream_record import TableFundStreamRecord
from dao.dao_user import DaoUser
from dao.dao_fund_stream_record import DaoFundStreamRecord


def parse_args():
    if len(sys.argv) == 2 and \
            (sys.argv[1] == "--version" or sys.argv[1] == "-v"):
        print("{}".format(__version__.__version__))
        sys.exit(0)

    opts, args = getopt.getopt(sys.argv[1:], "f:", ["config"])
    config_file = None
    for opt, arg in opts:
        if opt in ("-f", "--config"):
            config_file = arg

    if config_file is None:
        print("usage: {} -f config_file".format(sys.argv[0]))
        sys.exit(1)

    return config_file


def new_users(source, batch):
    """
    插入测试用户
    :param source:
    :param batch:
    :return:
    """
    users = [
        DaoUser(name="foo0", user_type=1, status=1),
        DaoUser(name="foo1", user_type=1, status=1),
        DaoUser(name="foo2", user_type=1, status=1),
        DaoUser(name="foo3", user_type=1, status=1),
        DaoUser(name="foo4", user_type=1, status=1),
        DaoUser(name="foo5", user_type=1, status=1),
        DaoUser(name="foo6", user_type=1, status=1),
        DaoUser(name="foo7", user_type=1, status=1),
        DaoUser(name="foo8", user_type=1, status=1),
        DaoUser(name="foo9", user_type=1, status=1),
    ]
    table_user = TableUser(table_name="t_user")
    table_user.insert(source=source, daos=users, batch_cnt=batch)


def sync(source_from, source_to):
    """
    同步两张表
    :param source_from:
    :param source_to:
    :return:
    """
    table_user = TableUser(table_name="t_user")
    daos = table_user.query(source=source_from)
    backup_table_user = TableUser(table_name="t_backup_user")
    backup_table_user.insert(source=source_to, daos=daos, batch_cnt=cfg.options_batch)


def new_fund_stream_records(source, batch):
    """
    插入资金流水
    :param source:
    :param batch:
    :return:
    """
    records = [
        DaoFundStreamRecord(id=1, user_id=1001, fund_dir=1, amount=1000.0),
        DaoFundStreamRecord(id=2, user_id=1001, fund_dir=1, amount=1000.0),
        DaoFundStreamRecord(id=3, user_id=1001, fund_dir=1, amount=1000.0),
        DaoFundStreamRecord(id=4, user_id=1001, fund_dir=1, amount=1000.0),
        DaoFundStreamRecord(id=5, user_id=1001, fund_dir=1, amount=1000.0),
        DaoFundStreamRecord(id=6, user_id=1001, fund_dir=1, amount=1000.0),
        DaoFundStreamRecord(id=7, user_id=1001, fund_dir=1, amount=1000.0),
        DaoFundStreamRecord(id=8, user_id=1001, fund_dir=1, amount=1000.0),
        DaoFundStreamRecord(id=9, user_id=1002, fund_dir=1, amount=1000.0),
        DaoFundStreamRecord(id=10, user_id=1002, fund_dir=1, amount=1000.0),
        DaoFundStreamRecord(id=11, user_id=1002, fund_dir=1, amount=1000.0),
    ]
    table_fund_record = TableFundStreamRecord(table_name="t_fund_stream_record")
    table_fund_record.insert(source=source, records=records, batch_cnt=batch)


def update_fund_stream_records(source):
    """
    更新资金流水
    """
    table_fund_record = TableFundStreamRecord(table_name="t_fund_stream_record")
    record = DaoFundStreamRecord(id=5, user_id=1002, fund_dir=1, amount=8000.0)
    affect_row = table_fund_record.update_amount(
        source=source,
        record=record)
    logging.info("update fund stream record, id: {}, amount: {}, affect row: {}".format(
        record.id, record.amount, affect_row))


class FundRecordHandler(object):
    """
    处理资金流水的任务
    """

    def __init__(self):
        self._head = {}
        self._id = None
        self._user_id = None
        self._dir = None
        self._amount = None

    def on_head(self, head):
        """
        当读取到表头
        :param row:
        """
        cnt = 0
        for val in head:
            self._head[val] = cnt
            cnt += 1
        self._id = self._head["id"]
        self._user_id = self._head["user_id"]
        self._dir = self._head["dir"]
        self._amount = self._head["amount"]

    def on_row(self, row, extra_info):
        """
        当读取到行
        :param row:
        :param extra_info:
        """
        logging.info("{} | on row: id={}, user_id={}, dir={}, amount={}".format(
            extra_info, row[self._id], row[self._user_id], row[self._dir], row[self._amount]))

    def on_dict_row(self, row, extra_info):
        """
        当读取到字典行
        :param row:
        :param extra_info:
        """
        logging.info("{} | on row: id={}, user_id={}, dir={}, amount={}".format(
            extra_info,
            row.get("id", None),
            row.get("user_id", None),
            row.get("dir", None),
            row.get("amount", None)))

    def on_dao(self, dao, extra_info):
        """
        当读取到对象
        """
        logging.info("{} | on row: id={}, user_id={}, dir={}, amount={}".format(
            extra_info,
            dao.id,
            dao.user_id,
            dao.dir,
            dao.amount))


def load_fund_stream_records(source, extra_info):
    """
    读取资金流水
    """
    handler = FundRecordHandler()
    table_fund_record = TableFundStreamRecord(table_name="t_fund_stream_record")

    daos = table_fund_record.query(source=source, user_id=1001)
    for dao in daos:
        handler.on_dao(dao=dao, extra_info=extra_info)


def ss_load_fund_stream_records(source, extra_info):
    """
    流式读取资金流水
    """
    handler = FundRecordHandler()
    table_fund_record = TableFundStreamRecord(table_name="t_fund_stream_record")

    head = table_fund_record.get_head(source=source)
    handler.on_head(head=head)

    table_fund_record.stream_fetch(
        source=source, callback=handler.on_row, user_id=1001, extra_info=extra_info)


def ss_dict_load_func_stream_records(source, extra_info):
    """
    流式字典读取资金流水
    """
    handler = FundRecordHandler()
    table_fund_record = TableFundStreamRecord(table_name="t_fund_stream_record")

    table_fund_record.stream_dict_fetch(
        source=source, callback=handler.on_dict_row, user_id=1001, extra_info=extra_info)


if __name__ == "__main__":
    # 获取配置文件路径
    config_filepath = parse_args()
    if not os.path.exists(config_filepath):
        print("error - failed find config file: {}".format(config_filepath))
        sys.exit(1)

    # 读取配置文件
    cfg = HelloConfig()
    cfg.load(filepath=config_filepath)

    # 初始化日志
    log_filepath = os.path.join(cfg.log_path, "hello.log")
    LogHandle.init_log(log_filepath, console_level=cfg.log_console_level, file_level=cfg.log_file_level)

    logging.info("-----------------------------")
    logging.info("start hello example")

    # 加载数据源
    datasource_config = DataSourceConfig()
    datasource_config.load(filepath=cfg.data_source_cfg)

    # 数据源名称
    source_product = cfg.db_map["product"]
    source_backup = cfg.db_map["backup"]
    db_product = datasource_config.get(source=source_product)
    if db_product is None:
        logging.error("failed get product data source: name='product', source='{}'".format(db_product))
    db_backup = datasource_config.get(source=source_backup)
    if db_backup is None:
        logging.error("failed get backup data source: name='backup', source='{}'".format(db_backup))

    # 往product中插入用户
    new_users(source=source_product, batch=cfg.options_batch)

    # 从product中读出所有用户并插入backup中
    sync(source_from=source_product, source_to=source_backup)

    # 往product中插入资金流水
    new_fund_stream_records(source=source_product, batch=cfg.options_batch)

    # 更新资金流水
    update_fund_stream_records(source=source_product)

    # 读取资金流水
    load_fund_stream_records(
        source=source_product, extra_info="yo~ fetch")

    # 流式读取资金流水
    ss_load_fund_stream_records(
        source=source_product, extra_info="yo~ stream fetch")

    # 流式字典读取资金硫酸
    ss_dict_load_func_stream_records(
        source=source_product, extra_info="yo~ stream dict fetch")
