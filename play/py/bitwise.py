class BitwiseHandle(object):
    """
    位域处理器
    """

    @classmethod
    def field_value(cls, value, offset, width):
        """
        获取指定位域的值
        :param value: uint64值
        :param offset: 偏移量
        :param width: 宽度
        :return: 位域值
        """
        value = value >> offset
        mask = (1 << width) - 1
        return value & mask


class Sequence(object):
    """
    序列号
    """

    def __init__(self, value: int):
        """
        解析位域
        struct sequence
        {
            uint32_t serv_id: 8;
            uint32_t ticket: 20;
            uint32_t seqtype: 4;
            uint32_t ymd: 28;
            uint32_t reserve: 4;
        };
        :param value: 整体的值
        """
        self.serv_id = BitwiseHandle.field_value(value=value, offset=0, width=8)
        self.ticket = BitwiseHandle.field_value(value=value, offset=8, width=20)
        self.seqtype = BitwiseHandle.field_value(value=value, offset=28, width=4)
        self.ymd = BitwiseHandle.field_value(value=value, offset=32, width=28)
        self.reserve = BitwiseHandle.field_value(value=value, offset=60, width=4)


if __name__ == "__main__":
    val = 86806125185597441
    seq = Sequence(value=val)
    print(
        "seq[{}]: "
        "serv_id={}, "
        "ticket={}, "
        "seqtype={}, "
        "ymd={}, "
        "reserve={}".format(
            val,
            seq.serv_id,
            seq.ticket,
            seq.seqtype,
            seq.ymd,
            seq.reserve))
