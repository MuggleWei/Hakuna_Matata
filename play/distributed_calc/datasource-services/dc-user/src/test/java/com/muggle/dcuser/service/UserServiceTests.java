package com.muggle.dcuser.service;

import com.muggle.common.utils.ErrorIdUtils;
import com.muggle.dcuser.mapper.AuthorityMapper;
import com.muggle.dcuser.model.Authority;
import com.muggle.dcuser.model.User;
import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.test.context.jdbc.Sql;
import org.springframework.test.context.jdbc.SqlGroup;

@SpringBootTest
@SqlGroup({
        @Sql(scripts = "/test_user.sql", executionPhase = Sql.ExecutionPhase.BEFORE_TEST_METHOD)
})
public class UserServiceTests {
    private final Logger logger = LoggerFactory.getLogger(this.getClass());

    @Autowired
    private UserService userService;

    @Autowired
    private AuthorityMapper authorityMapper;

    @Autowired
    private ErrorIdUtils errorIdUtils;

    @Test
    public void testQueryUser() {
        Long userId1 = 10001L;
        String userName1 = "test_user1";
        int status1 = 0;

        Long userId2 = 10002L;
        String userName2 = "test_user2";
        int status2 = 1;

        // query by id
        {
            User user = new User();
            user.setId(userId1);
            User rspUser = userService.queryUser(user);
            Assertions.assertNotNull(rspUser);
            Assertions.assertEquals(rspUser.getId(), userId1);
            Assertions.assertEquals(rspUser.getName(), userName1);
            Assertions.assertNull(rspUser.getPassword());
            Assertions.assertEquals(rspUser.getStatus(), status1);
        }

        // query by name
        {
            User user = new User();
            user.setName(userName2);
            User rspUser = userService.queryUser(user);
            Assertions.assertNotNull(rspUser);
            Assertions.assertEquals(rspUser.getId(), userId2);
            Assertions.assertEquals(rspUser.getName(), userName2);
            Assertions.assertNull(rspUser.getPassword());
            Assertions.assertEquals(rspUser.getStatus(), status2);
        }

        // query user not exists id
        {
            User user = new User();
            user.setId(10010L);
            User rspUser = userService.queryUser(user);
            Assertions.assertNull(rspUser);
        }

        // query user not exists name'
        {
            User user = new User();
            user.setName("test_user_not_exists");
            User rspUser = userService.queryUser(user);
            Assertions.assertNull(rspUser);
        }
    }

    @Test
    public void testQueryAuthority() {
        Long userId = 10001L;
        Integer numTasks = 1;

        // query by id
        {
            Authority authority = userService.queryUserAuthority(userId);
            Assertions.assertNotNull(authority);
            Assertions.assertEquals(authority.getUserId(), userId);
            Assertions.assertEquals(authority.getNumTasks(), numTasks);
        }

        // query user not exists id
        {
            Authority authority = userService.queryUserAuthority(10005L);
            Assertions.assertNull(authority);
        }
    }

    @Test
    public void testCheckUserPassword() {
        String userName = "test_user";
        String plainPassword = "xyz123456abc";
        String errPlainPassword = "xyz123456abC";

        // add new user
        {
            User user = new User();
            user.setName(userName);
            user.setPassword(plainPassword);
            int ret = userService.addUser(user);
            Assertions.assertEquals(ret, errorIdUtils.ok);

            user.setPassword(plainPassword);
            ret = userService.checkUserPassword(user);
            Assertions.assertEquals(ret, errorIdUtils.ok);

            user.setPassword(errPlainPassword);
            ret = userService.checkUserPassword(user);
            Assertions.assertEquals(ret, errorIdUtils.errIdPassword);
        }
    }

    @Test
    public void testAddUser() {
        String userName = "test_user";
        String plainPassword = "xyz123456abc";
        Long nextUserId = 0L;

        // successful add user
        {
            User user = new User();
            user.setName(userName);
            user.setPassword(plainPassword);
            int ret = userService.addUser(user);
            Assertions.assertEquals(ret, errorIdUtils.ok);

            User rspUser = userService.queryUser(user);
            Assertions.assertEquals(user.getId(), user.getId());
            Assertions.assertEquals(user.getName(), user.getName());

            nextUserId = user.getId() + 1L;
        }

        userName = "test_user_authority_repeated";

        // failed add user cause authority
        {
            Authority authority = new Authority();
            authority.setUserId(nextUserId);
            authority.setNumTasks(1);
            int ret = authorityMapper.addAuthority(authority);
            Assertions.assertEquals(ret, 1);

            User user = new User();
            user.setName(userName);
            user.setPassword(plainPassword);
            try {
                ret = userService.addUser(user);
                Assertions.assertTrue(ret != errorIdUtils.ok);
            } catch (Exception e) {}

            user.setId(null);
            User rspUser = userService.queryUser(user);
            Assertions.assertNull(rspUser);
        }
    }

}
