import logging
import time

from base.db_service_base import DbServiceBase
from base.db_utils import db_utils_wrapper


class DailyTableService(DbServiceBase):
    def __init__(self):
        super().__init__()

    @db_utils_wrapper
    def create_daily_tables(self, cursor, template_suffix, need_copy_content=False, currdate=None):
        if currdate is None:
            currdate = time.strftime('%Y%m%d', time.localtime(time.time()))
        cursor.execute("show tables")
        table_names = [item[0] for item in cursor.fetchall()]
        copy_tables = []
        for table_name in table_names:
            if table_name[-len(template_suffix):] == template_suffix:
                copy_tables.append(table_name)
        logging.debug("daily tables: {0}".format(copy_tables))

        for table in copy_tables:
            old_name = table
            new_name = table[:-len(template_suffix)] + currdate
            try:
                sql_str = "drop table if exists {0}".format(new_name)
                logging.debug(sql_str)
                cursor.execute(sql_str)

                sql_str = "create table {0} like {1}".format(new_name, old_name)
                logging.debug(sql_str)
                cursor.execute(sql_str)

                if need_copy_content is True:
                    sql_str = "insert into {0} select * from {1}".format(new_name, old_name)
                    logging.debug(sql_str)
                    cursor.execute(sql_str)
            except:
                logging.warning("failed init daily table {}".format(old_name))
                return False
        return True
