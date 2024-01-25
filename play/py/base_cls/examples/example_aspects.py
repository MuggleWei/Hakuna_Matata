import logging

from base.aspects import decorate_catch_except, decorate_func_call
from base.log_handle import LogHandle


@decorate_func_call
def say_hello(s):
    """
    say hello
    :param s: hello target
    """
    logging.info("hello {}".format(s))


@decorate_catch_except
@decorate_func_call
def throw_except():
    logging.info("throw except")
    raise Exception("I'm exception")


if __name__ == "__main__":
    LogHandle.init_log("logs/example_aspects.log", console_level=logging.DEBUG)

    say_hello("world")

    throw_except()
