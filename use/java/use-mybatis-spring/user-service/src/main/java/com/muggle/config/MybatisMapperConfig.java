package com.muggle.config;

import org.mybatis.spring.annotation.MapperScan;
import org.springframework.context.annotation.Configuration;

@Configuration
@MapperScan(basePackages = "com.muggle.mapper")
public class MybatisMapperConfig {
}
