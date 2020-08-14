package com.muggle.mapper;

import com.muggle.entity.User;
import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.extension.ExtendWith;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.test.context.ActiveProfiles;
import org.springframework.test.context.ContextConfiguration;
import org.springframework.test.context.junit.jupiter.SpringExtension;

import java.util.List;

@ExtendWith(SpringExtension.class)
@ContextConfiguration("/test.xml") // jdbc:initialize-database will init db and tables
@ActiveProfiles("dev")
public class TestUserMapper {
    private static final Logger logger = LoggerFactory.getLogger(TestUserMapper.class);

    @Autowired
    private UserMapper userMapper;

    private Long testUserId = 10001L;
    private String testUserName = "testuser1";
    private int testUserStatus = 0;
    private String testUserPassword = "123456";

    @Value("${server.address}")
    private String serviceAddress;

    @Test
    public void testQueryUserById() {
        User user = new User();
        user.setId(testUserId);
        List<User> users = userMapper.queryUser(user);
        Assertions.assertEquals(users.size(), 1);

        user = users.get(0);
        Assertions.assertEquals(user.getId(), testUserId);
        Assertions.assertEquals(user.getName(), testUserName);
        Assertions.assertEquals(user.getStatus(), testUserStatus);
        Assertions.assertEquals(user.getPassword(), testUserPassword);

        // query not exists id
        user = new User();
        user.setId(1000L);
        users = userMapper.queryUser(user);
        Assertions.assertEquals(users.size(), 0);
    }

    @Test
    public void testQueryUserByName() {
        User user = new User();
        user.setName(testUserName);
        List<User> users = userMapper.queryUser(user);
        Assertions.assertEquals(users.size(), 1);

        user = users.get(0);
        Assertions.assertEquals(user.getId(), testUserId);
        Assertions.assertEquals(user.getName(), testUserName);
        Assertions.assertEquals(user.getStatus(), testUserStatus);
        Assertions.assertEquals(user.getPassword(), testUserPassword);

        // query not exists name
        user = new User();
        user.setName("notexists");
        users = userMapper.queryUser(user);
        Assertions.assertEquals(users.size(), 0);
    }

    @Test
    public void testAddUser() {
        User user = new User();
        user.setName("testadduser");
        user.setPassword("123$%^abc");
        user.setStatus(1);

        int addCnt = userMapper.addUser(user);
        Assertions.assertEquals(addCnt, 1);
        Assertions.assertNotNull(user.getId());
        Assertions.assertTrue(user.getId() > 10001L);

        List<User> users = userMapper.queryUser(user);
        Assertions.assertEquals(users.size(), 1);
        Assertions.assertEquals(users.get(0).getId(), user.getId());
        Assertions.assertEquals(users.get(0).getName(), user.getName());
        Assertions.assertEquals(users.get(0).getStatus(), user.getStatus());
        Assertions.assertEquals(users.get(0).getPassword(), user.getPassword());
    }

    @Test
    public void testAddDuplicateUserName() {
        User user = new User();
        user.setName(testUserName);
        user.setPassword("123$%^abc");
        user.setStatus(1);

        String exceptionStr = null;
        try {
            int addCnt = userMapper.addUser(user);
        } catch (Exception e) {
            exceptionStr = e.getMessage();
        }
        Assertions.assertNotNull(exceptionStr);
    }

    @Test
    public void testDelUser() {
        User user = new User();
        user.setName("testdeluser");
        user.setPassword("123$%^abc");
        user.setStatus(1);

        int addCnt = userMapper.addUser(user);
        Assertions.assertEquals(addCnt, 1);
        Assertions.assertNotNull(user.getId());
        Assertions.assertTrue(user.getId() > 10001L);

        int delCnt = userMapper.delUser(user);
        Assertions.assertEquals(delCnt, 1);

        List<User> users = userMapper.queryUser(user);
        Assertions.assertEquals(users.size(), 0);
    }

    @Test
    public void testUpdateUserById() {
        User user = new User();

        // get id
        user.setName("testuser2");
        List<User> users = userMapper.queryUser(user);
        Long userId = users.get(0).getId();

        // update name by id
        user = new User();
        user.setName("testuser2updatename");
        int updateCnt = userMapper.updateUser(userId, null, user);
        Assertions.assertEquals(updateCnt, 1);

        // update status by id
        user = new User();
        user.setStatus(999);
        updateCnt = userMapper.updateUser(userId, null, user);
        Assertions.assertEquals(updateCnt, 1);

        // update password by id
        user = new User();
        user.setPassword("123$%^abc");
        updateCnt = userMapper.updateUser(userId, null, user);
        Assertions.assertEquals(updateCnt, 1);

        // query user
        user.setId(userId);
        users = userMapper.queryUser(user);
        user = users.get(0);
        Assertions.assertEquals(user.getId(), userId);
        Assertions.assertEquals(user.getName(), "testuser2updatename");
        Assertions.assertEquals(user.getStatus(), 999);
        Assertions.assertEquals(user.getPassword(), "123$%^abc");
    }

    @Test
    public void testUpdateUserByName() {
        User user = new User();

        // get id
        user.setName("testuser3");
        List<User> users = userMapper.queryUser(user);
        Long userId = users.get(0).getId();

        // update name by name
        user = new User();
        user.setName("testuser3updatename");
        int updateCnt = userMapper.updateUser(null, "testuser3", user);
        Assertions.assertEquals(updateCnt, 1);

        // update status by name
        user = new User();
        user.setStatus(999);
        updateCnt = userMapper.updateUser(null, "testuser3updatename", user);
        Assertions.assertEquals(updateCnt, 1);

        // update password by name
        user = new User();
        user.setPassword("123$%^abc");
        updateCnt = userMapper.updateUser(null, "testuser3updatename", user);
        Assertions.assertEquals(updateCnt, 1);

        // query user
        user.setId(userId);
        users = userMapper.queryUser(user);
        user = users.get(0);
        Assertions.assertEquals(user.getId(), userId);
        Assertions.assertEquals(user.getName(), "testuser3updatename");
        Assertions.assertEquals(user.getStatus(), 999);
        Assertions.assertEquals(user.getPassword(), "123$%^abc");
    }
}