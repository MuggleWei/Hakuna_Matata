class DaoUser(object):
    def __init__(
            self,
            id: int = None, name: str = None,
            user_type: int = None, status: int = None):
        """
        用户Dao
        :param id: 用户id
        :param name: 用户名
        :param user_type: 用户类型
        :param status: 用户状态
        """
        self._id = id
        self._name = name
        self._user_type = user_type
        self._status = status

    @property
    def id(self):
        return self._id

    @property
    def name(self):
        return self._name

    @property
    def user_type(self):
        return self._user_type

    @property
    def status(self):
        return self._status
