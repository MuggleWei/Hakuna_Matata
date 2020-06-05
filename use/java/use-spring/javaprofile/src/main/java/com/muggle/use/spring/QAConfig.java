package com.muggle.use.spring;

import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.context.annotation.Profile;

import java.util.Properties;

@Configuration
@Profile("qa")
public class QAConfig {

    @Bean
    public MyDataSourceProperties getProperties() {
        Properties properties = new Properties();
        properties.setProperty("configName", "qa");
        properties.setProperty("url", "jdbc:mysql://192.168.0.1:6666/sampledb?useUnicode=true&characterEncoding=utf8");
        properties.setProperty("username", "aqtest");
        properties.setProperty("password", "123456");

        MyDataSourceProperties dataSourceProperties = new MyDataSourceProperties();
        dataSourceProperties.setProperties(properties);
        return dataSourceProperties;
    }
}
