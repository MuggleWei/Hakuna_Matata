import logging

from base.log_handle import LogHandle
from base.singleton import singleton


@singleton
class Foo:
    def __init__(self, x):
        self.x = x


if __name__ == "__main__":
    LogHandle.init_log("logs/example_singleton.log", console_level=logging.DEBUG)

    f1 = Foo(10)
    f2 = Foo(20)
    logging.info("{}".format(f1.x))
    logging.info("{}".format(f2.x))
    logging.info(f1 is f2)
