package com.muggle.use.spring.config;

import com.muggle.use.spring.CalcTask;
import org.springframework.context.annotation.ComponentScan;
import org.springframework.context.annotation.Configuration;

@Configuration
@ComponentScan(basePackageClasses = CalcTask.class)
public class ScopeConfig {
}
