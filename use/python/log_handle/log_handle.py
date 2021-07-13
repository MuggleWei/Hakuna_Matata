import logging
import logging.handlers
import os


class LogHandle(object):
    """简单的用于初始化日志类"""

    @staticmethod
    def init_log(filename, console_level=logging.WARNING, file_level=logging.DEBUG, use_rotate=False, mode="a"):
        """
        初始化日志
        :param filename: 日志的输出路径
        :param console_level: 控制台日志等级
        :param file_level: 文件日志等级
        :param use_rotate: 是否使用旋转日志 True-使用rotating作为文件日志 False-使用file作为文件日志
        :param mode: 文件日志的打开模式
        :return:
        """
        # 生成日志输出目录
        folder = os.path.dirname(filename)
        if len(folder) > 0 and (not os.path.exists(folder)):
            os.makedirs(folder, exist_ok=True)

        # 获取格式化
        formatter = LogHandle.get_formatter()

        # 获取日志控制器
        ch = LogHandle.get_console_handler(console_level)
        if use_rotate is True:
            fh = LogHandle.get_rotating_handler(level=file_level, filename=filename, mode=mode)
        else:
            fh = LogHandle.get_file_handler(level=file_level, filename=filename, mode=mode)

        ch.setFormatter(formatter)
        fh.setFormatter(formatter)

        # 将控制器加入logger中
        logger = logging.getLogger()
        logger.setLevel(logging.DEBUG)
        logger.addHandler(ch)
        logger.addHandler(fh)

    @staticmethod
    def get_formatter():
        """
        获取日志固定的格式
        """
        return logging.Formatter("%(asctime)s|%(name)s|%(levelname)s|%(filename)s:%(lineno)s - %(message)s")

    @staticmethod
    def get_console_handler(level):
        """
        获取控制台日志处理器
        :param level: 日志过滤等级
        :return: 控制台日志处理器
        """
        handler = logging.StreamHandler()
        handler.setLevel(level)
        return handler

    @staticmethod
    def get_file_handler(level, filename, mode="a"):
        """
        获取文件日志处理器
        :param level: 日志过滤等级
        :param filename: 文件路径
        :param mode: 文件打开模式
        :return: 文件日志处理器
        """
        handler = logging.FileHandler(filename=filename, mode=mode)
        handler.setLevel(level)
        return handler

    @staticmethod
    def get_rotating_handler(level, filename, mode="a", maxBytes=20 * 1024 * 1024, backupCount=10):
        """
        获取自动旋转文件日志
        :param level: 日志过滤等级
        :param filename: 文件路径
        :param mode: 文件打开模式
        :param maxBytes: 单个文件最大大小
        :param backupCount: 最多保留多少文件
        :return:
        """
        handler = logging.handlers.RotatingFileHandler(
            filename=filename, mode=mode, maxBytes=maxBytes, backupCount=backupCount)
        handler.setLevel(level)
        return handler
