package com.muggle.config;

import org.mybatis.spring.mapper.MapperScannerConfigurer;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

@Configuration
public class MapperConfig {

//    @Bean
//    public AccountMapper accountMapper(SqlSessionFactory sqlSessionFactory) throws Exception {
//        SqlSessionTemplate sessionTemplate = new SqlSessionTemplate(sqlSessionFactory);
//        return sessionTemplate.getMapper(AccountMapper.class);
//    }

    @Bean
    static public MapperScannerConfigurer mapperScannerConfigurer() {
        MapperScannerConfigurer mapperScannerConfigurer = new MapperScannerConfigurer();
        mapperScannerConfigurer.setSqlSessionFactoryBeanName("sqlSessionFactory");
        mapperScannerConfigurer.setBasePackage("com.muggle.mapper");
        return mapperScannerConfigurer;
    }

}
