import logging

from log_handle import LogHandle

if __name__ == "__main__":
    LogHandle.init_log(filename="logs/output.log", use_rotate=True)

    logging.debug("debug message")
    logging.info("info message")
    logging.warning("warning message")
    logging.error("error message")
    logging.fatal("fatal message")
