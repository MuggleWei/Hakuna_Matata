package com.muggle.beanconfig;

import com.muggle.aop.PerformanceMonitor;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.ComponentScan;
import org.springframework.context.annotation.Configuration;
import org.springframework.context.annotation.EnableAspectJAutoProxy;

@Configuration
@ComponentScan(value = "com.muggle.object.*")
@EnableAspectJAutoProxy
public class AspectBeans {

    @Bean(value = "performanceAspect")
    public PerformanceMonitor getPerformanceMonitor() {
        return new PerformanceMonitor();
    }

}
