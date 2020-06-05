package com.muggle.use.spring;

import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.context.annotation.Profile;

import java.util.Properties;

@Configuration
@Profile("dev")
public class DevConfig {

    @Bean
    public MyDataSourceProperties getProperties() {
        Properties properties = new Properties();
        properties.setProperty("configName", "dev");
        properties.setProperty("url", "jdbc:mysql://127.0.0.1:23306/sampledb?useUnicode=true&characterEncoding=utf8");
        properties.setProperty("username", "muggle");
        properties.setProperty("password", "wsz123");

        MyDataSourceProperties dataSourceProperties = new MyDataSourceProperties();
        dataSourceProperties.setProperties(properties);
        return dataSourceProperties;
    }

}
