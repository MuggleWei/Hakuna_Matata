import getopt
import logging
import os
import sys
import time

from app.hello import __version__
from app.hello.hello_cfg import HelloConfig
from base.data_source_config import DataSourceConfig
from base.log_handle import LogHandle
from component.table_user import TableUser
from dao.dao_user import DaoUser


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
    daos = table_user.fetch_all(source=source_from)
    backup_table_user = TableUser(table_name="t_backup_user")
    backup_table_user.insert(source=source_to, daos=daos, batch_cnt=cfg.options_batch)


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
