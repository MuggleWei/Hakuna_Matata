class ConvBondCalculator(object):
    """
    可转债计算器
    这里的方法对应可转债的几个概念
    """

    def __init__(self):
        """
        初始化
        """
        self._price = 100.0  # 转债价格
        self._stk_price = 100.0  # 正股价格
        self._conv_price = 100.0  # 转股价格

    @property
    def price(self):
        return self._price

    @price.setter
    def price(self, price):
        self._price = price

    @property
    def stk_price(self):
        return self._stk_price

    @stk_price.setter
    def stk_price(self, stk_price):
        self._stk_price = stk_price

    @property
    def conv_price(self):
        return self._conv_price

    @conv_price.setter
    def conv_price(self, conv_price):
        self._conv_price = conv_price

    def conv_value(self):
        """
        转股价值:
            一张可转债按照转股价格转换成股票之后的价值

        转股价值 = (100 / 转股价格) * 股票现价
        :return:
        """
        return self.stk_price * (100.0 / self.conv_price)

    def premium_rate(self):
        """
        溢价率
            转债市场成交价与转股价值的比值; 反映的是你以市场价买入转债转股是亏是赚
              * 溢价率为正, 则亏
              * 溢价率为负, 则赚

        溢价率 = (转债价格 / 转股价值 - 1) * 100%
        :return:
        """
        return (self.price / self.conv_value()) - 1


if __name__ == '__main__':
    cal = ConvBondCalculator()
    cal.price = 102.89
    cal.stk_price = 16.05
    cal.conv_price = 23.290
    print("转债价格: {:.3f}".format(cal.price))
    print("正股价格: {:.3f}".format(cal.stk_price))
    print("转股价格: {:.3f}".format(cal.conv_price))
    print("转股价值: {:.3f}".format(cal.conv_value()))
    print("溢价率: {:.3f}%".format(100 * cal.premium_rate()))
