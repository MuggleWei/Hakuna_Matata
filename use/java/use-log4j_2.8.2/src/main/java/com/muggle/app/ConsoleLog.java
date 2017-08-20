package com.muggle.app;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

/**
 * Created by mugglewei on 2017/8/20.
 */
public class ConsoleLog {

    public static void main(String[] args) {
        Logger logger = LogManager.getLogger("myConsoleLog");
        logger.trace("trace level");
        logger.debug("debug level");
        logger.info("info level");
        logger.warn("warn level");
        logger.error("error level");
        logger.fatal("fatal level");
    }
}
