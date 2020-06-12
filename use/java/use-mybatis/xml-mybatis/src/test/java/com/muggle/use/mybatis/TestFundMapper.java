package com.muggle.use.mybatis;

import com.muggle.use.mybatis.mapper.FundMapper;
import com.muggle.use.mybatis.model.Fund;
import org.apache.ibatis.session.SqlSession;
import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;

import java.util.List;

public class TestFundMapper extends TestBaseMapper {
    @Test
    public void testQueryFund() {
        try (SqlSession sqlSession = getSqlSession()) {
            FundMapper FundMapper = sqlSession.getMapper(FundMapper.class);

            // query all
            List<Fund> Fund = FundMapper.queryFunds(null, null);
            Assertions.assertTrue(Fund.size() > 0);

            // query by user id
            Fund = FundMapper.queryFunds(1L, null);
            for (Fund fund : Fund) {
                Assertions.assertEquals(fund.getUserId(), 1L);
            }

            // query by currency type
            Fund = FundMapper.queryFunds(null, 0);
            for (Fund fund : Fund) {
                Assertions.assertEquals(fund.getCurrencyType(), 0);
            }

            // query by user and currency type
            Fund = FundMapper.queryFunds(1L, 0);
            for (Fund fund : Fund) {
                Assertions.assertEquals(fund.getUserId(), 1L);
                Assertions.assertEquals(fund.getCurrencyType(), 0);
            }
        }
    }
}
