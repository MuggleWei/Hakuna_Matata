import MySQLdb
import json
import logging

from base.singleton import singleton
from base.config_utils import ConfigUtils


@singleton
class DbUtils:
    def __init__(self, cfg_path):
        config_utils = ConfigUtils(cfg_path=cfg_path)
        db_config_path = config_utils.get_db_config_path()

        self.dbs = {}
        self.conns = {}
        with open(db_config_path, "r") as f:
            content = f.read()
            db_cfgs = json.loads(content)
            for cfg in db_cfgs:
                self.dbs[cfg['source']] = cfg
        logging.info("db configs: {0}".format(self.dbs))

    def __exit__(self, exc_type, exc_val, exc_tb):
        for conn in self.conns.values():
            conn.close()

    def get_db(self, source):
        """
        得到对应数据库信息
        :param source: 数据库source
        :return:
        """
        if source in self.dbs:
            return self.dbs[source]
        return None

    def get_conn(self, source):
        if source in self.conns:
            return self.conns[source]

        db = self.get_db(source)
        if db is None:
            logging.error("failed get db source: {0}".format(source))
            return None
        conn = MySQLdb.connect(
            host=db["ip"],
            port=db["port"],
            user=db["user"],
            passwd=db["password"],
            db=db["db"],
            charset='utf8'
        )
        self.conns[source] = conn
        return conn


def db_utils_wrapper(func):
    def wrapper(*args, **kwargs):
        db_utils = DbUtils(*args, **kwargs)
        config_utils = ConfigUtils()

        db_source = config_utils.get_db_source()
        conn = db_utils.get_conn(db_source)
        if conn is None:
            return None
        ret = None
        try:
            cursor = conn.cursor()
            kwargs['cursor'] = cursor

            ret = func(*args, **kwargs)

            conn.commit()
            cursor.close()
        except Exception as e:
            logging.exception('exception in use db utils')
            conn.rollback()
        return ret

    return wrapper


def get_cursor_rowcnt(cursor):
    if hasattr(cursor, 'rowcount'):
        return cursor.rowcount
    return 0
