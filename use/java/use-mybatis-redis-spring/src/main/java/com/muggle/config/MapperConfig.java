package com.muggle.config;

import com.muggle.mapper.AccountMapper;
import com.muggle.mapper.TradeRecordMapper;
import org.apache.ibatis.session.SqlSessionFactory;
import org.mybatis.spring.SqlSessionTemplate;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

@Configuration
public class MapperConfig {

    @Bean
    public AccountMapper accountMapper(SqlSessionFactory sqlSessionFactory) throws Exception {
        SqlSessionTemplate sessionTemplate = new SqlSessionTemplate(sqlSessionFactory);
        return sessionTemplate.getMapper(AccountMapper.class);
    }

    @Bean
    public TradeRecordMapper tradeRecordMapper(SqlSessionFactory sqlSessionFactory) throws Exception {
        SqlSessionTemplate sessionTemplate = new SqlSessionTemplate(sqlSessionFactory);
        return sessionTemplate.getMapper(TradeRecordMapper.class);
    }

}
