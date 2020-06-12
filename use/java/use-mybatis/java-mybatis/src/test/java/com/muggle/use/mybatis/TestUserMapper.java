package com.muggle.use.mybatis;


import com.muggle.use.mybatis.mapper.UserMapper;
import com.muggle.use.mybatis.model.User;
import org.apache.ibatis.session.SqlSession;
import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.MethodOrderer;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.TestMethodOrder;

import java.util.List;

@TestMethodOrder(value = MethodOrderer.OrderAnnotation.class)
public class TestUserMapper extends TestBaseMapper {

    @Test
    public void testSelectAll() {
        try (SqlSession sqlSession = getSqlSession()) {
            UserMapper userMapper = sqlSession.getMapper(UserMapper.class);
            List<User> users = userMapper.selectAll();
            Assertions.assertTrue(users.size() > 0);
            for (User user : users) {
                Assertions.assertNotNull(user.getUserId());
                Assertions.assertNotNull(user.getUserName());
                Assertions.assertNotNull(user.getRegDate());
                Assertions.assertNotNull(user.getRegIp());
            }
        }
    }

    @Test
    public void testSelectById() {
        try (SqlSession sqlSession = getSqlSession()) {
            long userId = 1;
            String userName = "testuser" + userId;

            UserMapper userMapper = sqlSession.getMapper(UserMapper.class);
            List<User> users = userMapper.selectById(userId);
            Assertions.assertEquals(users.size(), 1);
            User user = users.get(0);
            Assertions.assertEquals(user.getUserId(), userId);
            Assertions.assertEquals(user.getUserName(), userName);
        }
    }
}
