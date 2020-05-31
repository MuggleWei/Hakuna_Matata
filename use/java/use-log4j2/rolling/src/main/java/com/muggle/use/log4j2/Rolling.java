package com.muggle.use.log4j2;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

public class Rolling {
    public static void main(String[] args) {
        Logger logger = LogManager.getLogger("mylog");
        for (int i = 0; i < 50000; i++) {
            logger.trace("trace level " + i);
            logger.debug("debug level " + i);
            logger.info("info level " + i);
            logger.warn("warn level " + i);
            logger.error("error level " + i);
            logger.fatal("fatal level " + i);
        }
    }
}
