import json
import logging.config
import os
import sys

from base.singleton import singleton


@singleton
class ConfigUtils:
    def __init__(self, cfg_path=os.path.join(sys.argv[0], 'config')):
        # config files
        config_path = os.path.join(cfg_path, 'config.json')
        log_config_path = os.path.join(cfg_path, 'log.conf')
        db_config_path = os.path.join(cfg_path, 'db.json')

        logging.info("config path: " + config_path)
        logging.info("log config path: " + log_config_path)
        logging.info("db config path: " + db_config_path)

        # logging config
        if not os.path.exists("log"):
            os.makedirs("log")
        logging.config.fileConfig(log_config_path)

        # configs
        with open(config_path, 'r') as f:
            content = json.loads(f.read())

        # members
        self._db_config_path = db_config_path
        self._db_source = content['db_source']
        logging.info("use db source: " + self._db_source)

    def get_db_source(self):
        return self._db_source

    def get_db_config_path(self):
        return self._db_config_path
