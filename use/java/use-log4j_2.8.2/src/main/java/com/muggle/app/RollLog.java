package com.muggle.app;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

/**
 * Created by mugglewei on 2017/8/20.
 */
public class RollLog {

    private static Logger logger = null;

    public static void main(String[] args) {
        logger = LogManager.getLogger("myRollLog");
        for(int i = 0; i < 500000; i++) {
            PrintInfo();

            try {
                Thread.sleep(300);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    private static void PrintInfo() {
        logger.trace("trace level");
        logger.debug("debug level");
        logger.info("info level");
        logger.warn("warn level");
        logger.error("error level");
        logger.fatal("fatal level");
    }
}
