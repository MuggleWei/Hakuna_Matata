package com.muggle.use.mybatis;

import com.muggle.use.mybatis.mapper.FundMapper;
import com.muggle.use.mybatis.mapper.UserMapper;
import com.muggle.use.mybatis.model.Fund;
import com.muggle.use.mybatis.model.User;
import org.apache.ibatis.session.SqlSession;
import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;

import java.sql.Timestamp;
import java.util.List;

public class TestUserFund extends TestBaseMapper {
    @Test
    public void testSuccessInsertUserFund() {
        long userId = 0;

        try (SqlSession sqlSession = getSqlSession()) {
            UserMapper userMapper = sqlSession.getMapper(UserMapper.class);
            FundMapper FundMapper = sqlSession.getMapper(FundMapper.class);

            User user = new User();
            user.setUserName("testuserFund1");
            user.setRegDate(new Timestamp(System.currentTimeMillis()));
            user.setRegIp("127.0.0.1");

            int cnt = userMapper.insertUser(user);
            Assertions.assertEquals(cnt, 1);
            userId = user.getUserId();

            Fund Fund = new Fund();
            Fund.setUserId(user.getUserId());
            Fund.setCurrencyType(0);
            Fund.setAmount(10000L);

            cnt = FundMapper.insertFund(Fund);
            Assertions.assertEquals(cnt, 1);

            Fund.setCurrencyType(1);
            cnt = FundMapper.insertFund(Fund);
            Assertions.assertEquals(cnt, 1);

            sqlSession.commit();
        }

        try (SqlSession sqlSession = getSqlSession()) {
            UserMapper userMapper = sqlSession.getMapper(UserMapper.class);
            FundMapper fundMapper = sqlSession.getMapper(FundMapper.class);

            List<User> users = userMapper.findUserById(userId);
            Assertions.assertEquals(users.size(), 1);
            User user = users.get(0);

            List<Fund> Fund = fundMapper.queryFunds(user.getUserId(), null);
            Assertions.assertEquals(Fund.size(), 2);
        }
    }

    @Test
    public void testFailedInsertUserFunds() {
        long userId = 0;

        try (SqlSession sqlSession = getSqlSession()) {
            UserMapper userMapper = sqlSession.getMapper(UserMapper.class);
            FundMapper FundMapper = sqlSession.getMapper(FundMapper.class);

            User user = new User();
            user.setUserName("testuserFund2");
            user.setRegDate(new Timestamp(System.currentTimeMillis()));
            user.setRegIp("127.0.0.1");
            int cnt = userMapper.insertUser(user);
            Assertions.assertEquals(cnt, 1);
            userId = user.getUserId();

            Fund Fund = new Fund();
            Fund.setUserId(user.getUserId());
            Fund.setCurrencyType(0);
            Fund.setAmount(10000L);
            cnt = FundMapper.insertFund(Fund);
            Assertions.assertEquals(cnt, 1);

            cnt = FundMapper.insertFund(Fund);
            Assertions.assertEquals(cnt, 0);

            sqlSession.commit();
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }

        try (SqlSession sqlSession = getSqlSession()) {
            UserMapper userMapper = sqlSession.getMapper(UserMapper.class);
            FundMapper fundMapper = sqlSession.getMapper(FundMapper.class);

            List<User> users = userMapper.findUserById(userId);
            Assertions.assertEquals(users.size(), 0);

            List<Fund> funds = fundMapper.queryFunds(userId, null);
            Assertions.assertEquals(funds.size(), 0);
        }
    }
}
