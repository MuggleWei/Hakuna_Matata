package com.muggle.use.spring;

import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.context.annotation.Profile;

import java.util.Properties;

@Configuration
@Profile("prod")
public class ProdConfig {

    @Bean
    public MyDataSourceProperties getProperties() {
        Properties properties = new Properties();
        properties.setProperty("configName", "prod");
        properties.setProperty("url", "*");
        properties.setProperty("username", "*");
        properties.setProperty("password", "*");

        MyDataSourceProperties dataSourceProperties = new MyDataSourceProperties();
        dataSourceProperties.setProperties(properties);
        return dataSourceProperties;
    }
}
