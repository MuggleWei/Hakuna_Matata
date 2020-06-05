package com.muggle.use.spring;

import com.muggle.use.spring.config.ProfileConfig;
import org.springframework.context.annotation.AnnotationConfigApplicationContext;

public class ProfileMain {
    public static void main(String[] args) {
        // jar-with-dependencies: java -Dspring.profiles.active=[dev|qa|prod] -jar xxx.jar
        // jar windows: java -Dspring.profiles.active=dev -cp xxx.jar;lib\* com.muggle.use.spring.ProfileMain
        // jar linux: java -Dspring.profiles.active=dev -cp xxx.jar:lib/* com.muggle.use.spring.ProfileMain
        AnnotationConfigApplicationContext context = new AnnotationConfigApplicationContext(ProfileConfig.class);

        MyDataSourceProperties dataSourceProperties = context.getBean(MyDataSourceProperties.class);
        System.out.println("data source config name: " + dataSourceProperties.getProperties().get("configName"));

        context.close();
    }
}
