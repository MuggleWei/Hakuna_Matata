import logging


def decorate_func_call(func):
    """
    函数调用监控装饰器
    """
    def wrapper(*args, **kwargs):
        logging.info("func call begin: {}".format(func.__name__))
        ret = func(*args, **kwargs)
        logging.info("func call end: {}".format(func.__name__))
        return ret
    return wrapper


def decorate_catch_except(func):
    """
    函数异常捕获装饰器
    """
    def wrapper(*args, **kwargs):
        try:
            ret = func(*args, **kwargs)
        except Exception as e:
            logging.error(
                "catch exception: {}".format(e))
            return None
        return ret
    return wrapper
