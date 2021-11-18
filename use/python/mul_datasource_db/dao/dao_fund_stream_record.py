class DaoFundStreamRecord(object):
    def __init__(
            self,
            id: int = None,
            user_id: int = None,
            fund_dir: int = None,
            amount: float = None):
        """
        资金流水Dao
        :param id: 资金流水id
        :param user_id: 用户id
        :param fund_dir: 资金流向
        :param amount: 资金量
        """
        self._id = id
        self._user_id = user_id
        self._dir = fund_dir
        self._amount = amount

    @property
    def id(self):
        return self._id

    @property
    def user_id(self):
        return self._user_id

    @property
    def dir(self):
        return self._dir

    @property
    def amount(self):
        return self._amount
