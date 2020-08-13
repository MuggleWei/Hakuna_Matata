package com.muggle.mapper;

import com.muggle.entity.User;
import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.extension.ExtendWith;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.test.context.ContextConfiguration;
import org.springframework.test.context.junit.jupiter.SpringExtension;

import java.util.List;

@ExtendWith(SpringExtension.class)
@ContextConfiguration("/test.xml") // jdbc:initialize-database will init db and tables
public class TestUserMapper {
    private static final Logger logger = LoggerFactory.getLogger(TestUserMapper.class);

    @Autowired
    private UserMapper userMapper;

    @Test
    public void testQueryUserByName() {
        User user = new User();

        user.setName("notexists");
        List<User> users = userMapper.queryUser(user);
        Assertions.assertEquals(users.size(), 0);

        String userName = "testuser0";
        user.setName(userName);
        users = userMapper.queryUser(user);
        Assertions.assertEquals(users.size(), 1);
        user = users.get(0);
        Assertions.assertEquals(user.getName(), userName);
    }
}