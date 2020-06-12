package com.muggle.use.mybatis;


import com.muggle.use.mybatis.mapper.UserMapper;
import com.muggle.use.mybatis.model.User;
import org.apache.ibatis.session.SqlSession;
import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.MethodOrderer;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.TestMethodOrder;

import java.sql.Timestamp;
import java.util.ArrayList;
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
            List<User> users = userMapper.findUserById(userId);
            Assertions.assertEquals(users.size(), 1);
            User user = users.get(0);
            Assertions.assertEquals(user.getUserId(), userId);
            Assertions.assertEquals(user.getUserName(), userName);
        }
    }

    @Test
    public void testFindUser() {
        try (SqlSession sqlSession = getSqlSession()) {
            String userName = "testuser1";
            String regIp = "127.0.0.1";

            UserMapper userMapper = sqlSession.getMapper(UserMapper.class);

            List<User> users = userMapper.findUser(userName, regIp);
            Assertions.assertEquals(users.size(), 1);

            users = userMapper.findUser(userName, null);
            Assertions.assertEquals(users.size(), 1);

            users = userMapper.findUser(userName, "192.168.0.1");
            Assertions.assertEquals(users.size(), 0);
        }
    }

    @Test
    public void testInsertUser() {
        try (SqlSession sqlSession = getSqlSession()) {
            UserMapper userMapper = sqlSession.getMapper(UserMapper.class);
            User user = new User();
            user.setUserName("testuser_dev");
            user.setRegDate(new Timestamp(System.currentTimeMillis()));
            user.setRegIp("127.0.0.1");

            int insertCnt = userMapper.insertUser(user);
            Assertions.assertEquals(insertCnt, 1);
            Assertions.assertNotNull(user.getUserId());
            Assertions.assertTrue(user.getUserId() > 0);

            sqlSession.commit();
        }
    }

    @Test
    public void testInsertUserList() {
        List<User> users = new ArrayList<>();
        int cnt = 10;
        for (int i = 0; i < cnt; i++) {
            User user = new User();
            user.setUserName("testuser_dev" + i);
            user.setRegDate(new Timestamp(System.currentTimeMillis()));
            user.setRegIp("127.0.0.1");
            users.add(user);
        }

        try (SqlSession sqlSession = getSqlSession()) {
            UserMapper userMapper = sqlSession.getMapper(UserMapper.class);
            int insertCnt = userMapper.insertUserArray(users);
            Assertions.assertEquals(insertCnt, cnt);
            sqlSession.commit();
        }
    }

    @Test
    public void updateUser() {
        String newName = "testuser2_rename";
        Timestamp newTimestamp = new Timestamp(System.currentTimeMillis());
        String newIp = "192.168.0.1";

        try (SqlSession sqlSession = getSqlSession()) {
            UserMapper userMapper = sqlSession.getMapper(UserMapper.class);

            userMapper.updateUser(2L, "testuser2_rename", null, null);
            userMapper.updateUser(2L, null, newTimestamp, null);
            userMapper.updateUser(2L, null, null, newIp);

            sqlSession.commit();
        }

        try (SqlSession sqlSession = getSqlSession()) {
            UserMapper userMapper = sqlSession.getMapper(UserMapper.class);
            List<User> users = userMapper.findUserById(2L);
            Assertions.assertEquals(users.size(), 1);

            User user = users.get(0);
            Assertions.assertEquals(user.getUserName(), newName);
            // Assertions.assertEquals(user.getRegDate().getTime() / 1000, newTimestamp.getTime() / 1000);
            Assertions.assertEquals(user.getRegIp(), newIp);
        }
    }

    @Test
    public void testDeleteUser() {
        try (SqlSession sqlSession = getSqlSession()) {
            UserMapper userMapper = sqlSession.getMapper(UserMapper.class);

            int deleteCnt = 0;

            deleteCnt = userMapper.deleteUser(4L, null);
            Assertions.assertEquals(deleteCnt, 1);

            deleteCnt = userMapper.deleteUser(null, "testuser5");
            Assertions.assertEquals(deleteCnt, 1);

            deleteCnt = userMapper.deleteUser(null, null);
            Assertions.assertEquals(deleteCnt, 0);

            sqlSession.commit();
        }
    }
}
