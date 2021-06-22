class DataSource(object):
    """
    数据源
    """

    def __init__(
            self, source: str,
            ip: str, port: int,
            user: str, passwd: str,
            db: str, charset="utf8"):
        """
        数据源构造
        :param source: 数据源名
        :param ip: 数据源ip地址
        :param port: 数据源端口
        :param user: 数据源用户名
        :param passwd: 数据源密码
        :param db: 数据库名称
        :param charset: 数据库默认字符编码
        """
        self._source = source
        self._ip = ip
        self._port = int(port)
        self._user = user
        self._passwd = passwd
        self._db = db
        self._charset = charset

    @property
    def source(self):
        return self._source

    @property
    def ip(self):
        return self._ip

    @property
    def port(self):
        return self._port

    @property
    def user(self):
        return self._user

    @property
    def passwd(self):
        return self._passwd

    @property
    def db(self):
        return self._db

    @property
    def charset(self):
        return self._charset
