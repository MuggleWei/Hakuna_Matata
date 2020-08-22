import collections
import datetime
import logging
import time


class TimeTaskManager:
    """
    定时任务管理者
    """

    def __init__(self, tzinfo):
        """
        初始化定时任务管理者
        :param tzinfo: 时区信息, 为None代表使用UTC+0时区
        """
        self.task_callback = {}
        self.time_task_dict = {}
        if tzinfo is None:
            tzinfo = datetime.timezone(datetime.timedelta(hours=0))
        self.tzinfo = tzinfo

    def add_task(self, task_name, callback, daily_time_list):
        self.task_callback[task_name] = callback
        for daily_time in daily_time_list:
            if daily_time not in self.time_task_dict:
                self.time_task_dict[daily_time] = []
            self.time_task_dict[daily_time].append(task_name)

    def run(self):
        logging.info('-----------------------------------')
        logging.info('time task list: ')
        time_task_items = self.time_task_dict.items()
        od = collections.OrderedDict(sorted(time_task_items))
        for k, v in od.items():
            logging.info('{}  {}'.format(k, v))

        logging.info('-----------------------------------')
        logging.info('run time task manager')
        last_hm = self.get_time_hm(time.time() - 120)
        while True:
            hm = self.get_time_hm(time.time())
            if hm == last_hm:
                continue
            else:
                last_hm = hm
            if hm in self.time_task_dict.keys():
                for task_name in self.time_task_dict[hm]:
                    callback = self.task_callback.get(task_name, None)
                    if callback is None:
                        logging.error('failed find callback function for {}'.format(task_name))
                    logging.info('{}  run time task {}'.format(hm, task_name))
                    try:
                        callback()
                    except Exception as e:
                        logging.exception('hm={}, task_name={}'.format(hm, task_name))
            time.sleep(5)

    def get_time_hm(self, ts):
        dt = datetime.datetime.fromtimestamp(ts, tz=self.tzinfo)
        return dt.strftime('%H:%M')
