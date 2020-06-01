package com.muggle.use.log4j2;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.apache.logging.log4j.core.config.ConfigurationSource;
import org.apache.logging.log4j.core.config.Configurator;

import java.io.*;

public class CustomPath {
    public static void main(String[] args) {
        // read from command: -Dlog4j.configurationFile=config/log4j2.xml
        // set path from property: System.setProperty("log4j.configurationFile", "config/log4j2.xml");

//        // read from resource
//        try {
//            InputStream inputStream = CustomPath.class.getClassLoader().getResourceAsStream("config/log4j2.xml");
//            if (inputStream == null) {
//                System.out.println("failed load from resource config/log4j2.xml");
//                System.exit(1);
//            }
//            BufferedInputStream in = new BufferedInputStream(inputStream);
//            final ConfigurationSource source = new ConfigurationSource(in);
//            Configurator.initialize(null, source);
//        } catch (IOException e) {
//            e.printStackTrace();
//        }

        Logger logger = LogManager.getLogger("myAsyncLog");
        logger.trace("trace level");
        logger.debug("debug level");
        logger.info("info level");
        logger.warn("warn level");
        logger.error("error level");
        logger.fatal("fatal level");
    }
}
