import logging
import sys
from xml.dom.minidom import Element

from base.log_handle import LogHandle
from base.xml_config import XmlConfig


class HelloConfig(XmlConfig):
    def __init__(self):
        """ 初始化配置信息 """
        self.log_path = "log"
        self.log_console_level = logging.WARNING
        self.log_file_level = logging.INFO

        self.data_source_cfg: str = ""
        self.db_map = {}

        self.options_batch = 64

    def load(self, filepath):
        """
        加载配置文件
        :param filepath:
        :return:
        """
        dom = self.dom(filepath=filepath)
        root: Element = dom.documentElement

        # 初始化日志配置
        node_log_list = root.getElementsByTagName("log")
        if len(node_log_list) != 1:
            print("warning - failed find log node, use default config")
        else:
            self._init_log(node_log_list[0])

        # 初始化datasource配置
        data_source_list = root.getElementsByTagName("datasource")
        if len(data_source_list) != 1:
            print("error - failed load config - can't find datasource")
            sys.exit(1)
        self._init_datasource(data_source_list[0])

        # 初始化数据库映射
        db_map_list = root.getElementsByTagName("db_map")
        if len(db_map_list) != 1:
            print("error - failed load config - can't find db_config")
            sys.exit(1)
        self._init_db_map(db_map_list[0])

        # 初始化选项
        options_list = root.getElementsByTagName("options")
        if len(options_list) != 1:
            print("error - failed load config - can't find options")
            sys.exit(1)
        self._init_options(options_list[0])

    def _init_log(self, node_log):
        """
        获取日志配置
        :param node_log: 日志配置节点
        :return:
        """
        self.log_path = self.get_node_attr(node_log, "path", "logs")
        console_level = self.get_node_attr(node_log, "console_level", "warning")
        self.log_console_level = LogHandle.log_level(console_level)
        file_level = self.get_node_attr(node_log, "file_level", "info")
        self.log_file_level = LogHandle.log_level(file_level)

        print("info - log: path={}, console_level={}, file_level={}".format(
            self.log_path, console_level, file_level))

    def _init_datasource(self, node: Element):
        """
        获取数据源路径
        :return:
        """
        if not node.hasAttribute("filepath"):
            print("error - failed find datasource/filepath")
            sys.exit(1)
        self.data_source_cfg = self.get_node_attr(node, "filepath", "")
        print("info - data source path: {}".format(self.data_source_cfg))

    def _init_db_map(self, node: Element):
        """
        初始化数据库映射
        :param node:
        :return:
        """
        db_node_list = node.getElementsByTagName("db")
        for child in db_node_list:
            name = self.get_node_attr(child, "name", "")
            source = self.get_node_attr(child, "source", "")
            self.db_map[name] = source
            print("info - db: name={}, source={}".format(name, source))

    def _init_options(self, node: Element):
        """
        初始化选项
        :param node:
        :return:
        """
        if not node.hasAttribute("batch"):
            print("error - failed find options/batch")
            sys.exit(1)
        self.options_batch = self.get_node_attr(node, "batch", None)
        if self.options_batch is None:
            print("error - failed get options/batch attr")
            sys.exit(1)
        self.options_batch = int(self.options_batch)
        print("info - options: batch_cnt={}".format(self.options_batch))
