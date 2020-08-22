import datetime
import logging.config
import os
import time
from pathlib import Path

from time_task_manager import TimeTaskManager


def say_hello():
    tzinfo = datetime.timezone(datetime.timedelta(hours=8))
    ts = time.time()
    dt_utc8 = datetime.datetime.fromtimestamp(ts, tz=tzinfo)
    dt_utc = datetime.datetime.utcfromtimestamp(ts)
    print("hello [UTC]{} | [UTC+8]{}".format(dt_utc, dt_utc8))


def throw_except():
    raise Exception("throw except")


if __name__ == '__main__':
    if not os.path.exists("./log"):
        os.mkdir("./log")
    logging.config.fileConfig("config/time_tasks_log.cfg")

    tzinfo = datetime.timezone(datetime.timedelta(hours=8))
    time_task_manager = TimeTaskManager(tzinfo=tzinfo)

    cur_ts = time.time()
    hm_list = []
    for i in range(60):
        hm_list.append(time_task_manager.get_time_hm(cur_ts + i * 60))
    time_task_manager.add_task('say_hello', say_hello, hm_list)
    time_task_manager.add_task('throw_except', throw_except, hm_list)
    time_task_manager.run()
