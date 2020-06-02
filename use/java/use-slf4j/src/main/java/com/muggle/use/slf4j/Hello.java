package com.muggle.use.slf4j;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class Hello {
    private static final Logger logger = LoggerFactory.getLogger(Hello.class);

    public static void main(String[] args) {
        logger.trace("trace level");
        logger.debug("debug level");
        logger.info("info level");
        logger.warn("warn level");
        logger.error("error level");
    }
}
