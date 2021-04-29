import logging
import logging.config
import os


if __name__ == "__main__":
    cur_folder = os.path.dirname(os.path.abspath(__file__))
    log_cfg_path = os.path.join(cur_folder, "config", "log.conf")
    if not os.path.exists("logs"):
        os.mkdir("logs")
    logging.config.fileConfig(log_cfg_path)

    logging.debug("debug message")
    logging.info("info message")
    logging.warning("warning message")
    logging.error("error message")
    logging.fatal("fatal message")
