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

    @id.setter
    def id(self, val):
        self._id = val

    @property
    def name(self):
        return self._name

    @name.setter
    def name(self, val):
        self._name = val

    @property
    def user_type(self):
        return self._user_type

    @user_type.setter
    def user_type(self, val):
        self._user_type = val

    @property
    def status(self):
        return self._status

    @status.setter
    def status(self, val):
        self._status = val
