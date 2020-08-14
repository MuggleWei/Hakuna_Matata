package com.muggle.service;

import com.muggle.entity.Level;
import com.muggle.entity.User;
import com.muggle.mapper.LevelMapper;
import com.muggle.mapper.UserMapper;
import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.extension.ExtendWith;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.test.context.ActiveProfiles;
import org.springframework.test.context.ContextConfiguration;
import org.springframework.test.context.junit.jupiter.SpringExtension;

import java.util.List;

@ExtendWith(SpringExtension.class)
@ContextConfiguration("/test.xml") // jdbc:initialize-database will init db and tables
@ActiveProfiles("dev")
public class TestUserService {
    private static final Logger logger = LoggerFactory.getLogger(TestUserService.class);

    @Autowired
    private UserService userService;

    @Autowired
    private UserMapper userMapper;

    @Autowired
    private LevelMapper levelMapper;

    @Test
    public void testTransaction() {
        // add level
        Level level = new Level();
        level.setId(10100L);
        level.setLevel(0);
        levelMapper.addLevel(level);

        // duplicate add level id, catch exception
        User user = new User();
        user.setName("testuserservice");
        user.setPassword("123$%^abc");
        user.setStatus(1);

        String exceptionStr = null;
        try {
            userService.addUser(user, level);
        } catch (Exception e) {
            exceptionStr = e.getMessage();
        }
        Assertions.assertNotNull(exceptionStr);

        List<User> users = userMapper.queryUser(user);
        Assertions.assertEquals(users.size(), 0);

        // add user and level
        level.setId(10020L);
        int addCnt = userService.addUser(user, level);
        Assertions.assertEquals(addCnt, 1);

        users = userMapper.queryUser(user);
        Assertions.assertEquals(users.size(), 1);
    }
}
