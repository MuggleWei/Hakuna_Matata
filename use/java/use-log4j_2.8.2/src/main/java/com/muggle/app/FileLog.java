package com.muggle.app;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

/**
 * Created by mugglewei on 2017/8/20.
 */
public class FileLog {
    public static void main(String[] args) {
        // use below or -Dlog4j.configurationFile=config/log4j2.xml
        System.setProperty("log4j.configurationFile", "config/log4j2.xml");

        Logger logger = LogManager.getLogger("myFileLog");
        logger.trace("trace level");
        logger.debug("debug level");
        logger.info("info level");
        logger.warn("warn level");
        logger.error("error level");
        logger.fatal("fatal level");
    }
}
