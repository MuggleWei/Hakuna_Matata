package com.muggle.dcuser.config;

import org.apache.coyote.http11.AbstractHttp11Protocol;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.boot.web.embedded.tomcat.TomcatServletWebServerFactory;
import org.springframework.boot.web.server.WebServerFactoryCustomizer;
import org.springframework.context.annotation.Configuration;

@Configuration
public class TomcatCustomizer implements WebServerFactoryCustomizer<TomcatServletWebServerFactory> {
    private final Logger logger = LoggerFactory.getLogger(this.getClass());

    @Override
    public void customize(TomcatServletWebServerFactory factory) {
        factory.addConnectorCustomizers(connector -> {
            AbstractHttp11Protocol protocol = (AbstractHttp11Protocol) connector.getProtocolHandler();

            int originMaxKeepAliveRequests = protocol.getMaxKeepAliveRequests();
            int originKeepAliveTimeout = protocol.getKeepAliveTimeout();
            int originConnectionTimeout = protocol.getConnectionTimeout();
            int originMaxConnection = protocol.getMaxConnections();

            // https://httpd.apache.org/docs/2.4/mod/core.html
            // MaxKeepAliveRequests: If it is set to 0, unlimited requests will be allowed
            protocol.setMaxKeepAliveRequests(0);

            logger.info("####################################################################################");
            logger.info("#");
            logger.info("# TomcatCustomizer");
            logger.info("#");
            logger.info("# origin maxKeepAliveRequests: {}", originMaxKeepAliveRequests);
            logger.info("# origin keepalive timeout: {} ms", originKeepAliveTimeout);
            logger.info("# origin connection timeout: {} ms", originConnectionTimeout);
            logger.info("# origin max connections: {}", originMaxConnection);
            logger.info("# custom maxKeepAliveRequests: {}", protocol.getMaxKeepAliveRequests());
            logger.info("#");
            logger.info("####################################################################################");
        });
    }
}
