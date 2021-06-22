import json
import logging
import typing

from base.data_source import DataSource
from base.singleton import singleton


@singleton
class DataSourceConfig(object):
    def __init__(self):
        self._datasources: typing.Dict[str, DataSource] = {}

    def load(self, filepath):
        """
        读取数据源配置文件
        :param filepath: 数据源配置文件
        """
        with open(filepath, "r") as f:
            content = f.read()
        cfgs = json.loads(content)
        for item in cfgs:
            try:
                ds_item = DataSource(
                    source=item["source"],
                    ip=item["ip"],
                    port=item["port"],
                    user=item["user"],
                    passwd=item["password"],
                    db=item["db"],
                )
                self._datasources[ds_item.source] = ds_item
            except Exception as e:
                logging.warning("invalid data source item: {}, expect: {}".format(item, e))

    def get(self, source) -> DataSource:
        """
        获取数据源配置
        :param source: 数据源名
        :return:
        """
        return self._datasources.get(source, None)
