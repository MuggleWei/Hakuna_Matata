package com.muggle.dcuser.mapper;

import com.muggle.dcuser.model.User;
import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.test.context.jdbc.Sql;
import org.springframework.test.context.jdbc.SqlGroup;

import java.util.List;

@SpringBootTest
@SqlGroup({
        @Sql(scripts = "/test_user.sql", executionPhase = Sql.ExecutionPhase.BEFORE_TEST_METHOD)
})
public class UserMapperTests {
    private final Logger logger = LoggerFactory.getLogger(this.getClass());

    @Autowired
    private UserMapper userMapper;

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
            List<User> users = userMapper.queryUser(user);
            Assertions.assertEquals(users.size(), 1);
            Assertions.assertEquals(users.get(0).getId(), userId1);
            Assertions.assertEquals(users.get(0).getName(), userName1);
            Assertions.assertNull(users.get(0).getPassword());
            Assertions.assertEquals(users.get(0).getStatus(), status1);
        }

        // query by name
        {
            User user = new User();
            user.setName(userName2);
            List<User> users = userMapper.queryUser(user);
            Assertions.assertEquals(users.size(), 1);
            Assertions.assertEquals(users.get(0).getId(), userId2);
            Assertions.assertEquals(users.get(0).getName(), userName2);
            Assertions.assertNull(users.get(0).getPassword());
            Assertions.assertEquals(users.get(0).getStatus(), status2);
        }

        // query user not exists id
        {
            User user = new User();
            user.setId(10010L);
            List<User> users = userMapper.queryUser(user);
            Assertions.assertEquals(users.size(), 0);
        }

        // query user not exists name'
        {
            User user = new User();
            user.setName("test_user_not_exists");
            List<User> users = userMapper.queryUser(user);
            Assertions.assertEquals(users.size(), 0);
        }
    }

    @Test
    public void testQueryUserPassword() {
        Long userId = 10001L;
        String userName = "test_user1";
        String password = "123456abcABC";

        // query by id
        {
            User user = new User();
            user.setId(userId);
            List<User> users = userMapper.queryUserPassword(user);
            Assertions.assertEquals(users.size(), 1);
            Assertions.assertEquals(users.get(0).getId(), userId);
            Assertions.assertEquals(users.get(0).getPassword(), password);
        }

        // query by name
        {
            User user = new User();
            user.setName(userName);
            List<User> users = userMapper.queryUserPassword(user);
            Assertions.assertEquals(users.size(), 1);
            Assertions.assertEquals(users.get(0).getId(), userId);
            Assertions.assertEquals(users.get(0).getPassword(), password);
        }
    }

    @Test
    public void testAddUser() {
        String userName = "test_user";
        String password = "123456";

        // successful add user
        {
            User user = new User();
            user.setName(userName);
            user.setPassword(password);
            int addCnt = userMapper.addUser(user);
            Assertions.assertEquals(addCnt, 1);
            Assertions.assertNotNull(user.getId());
            Assertions.assertTrue(user.getId() > 10000L);
        }

        // failed add user, repeated name
        {
            User user = new User();
            user.setName(userName);
            user.setPassword(password);
            boolean catchException = false;
            try {
                userMapper.addUser(user);
            } catch (Exception e) {
                catchException = true;
            }
            Assertions.assertTrue(catchException);
        }
    }

    @Test
    public void testDelUser() {
        String delUserName = "test_user1";
        Long delUserId = 10002L;

        // successful delete user by id
        {
            User user = new User();
            user.setId(delUserId);
            int delCnt = userMapper.delUser(user);
            Assertions.assertEquals(delCnt, 1);
        }

        // successful delete user by name
        {
            User user = new User();
            user.setName(delUserName);
            int delCnt = userMapper.delUser(user);
            Assertions.assertEquals(delCnt, 1);
        }

        // delete with not exists id user
        {
            User user = new User();
            user.setId(10010L);
            int delCnt = userMapper.delUser(user);
            Assertions.assertEquals(delCnt, 0);
        }

        // delete with not exists name user
        {
            User user = new User();
            user.setName("test_user_not_exists");
            int delCnt = userMapper.delUser(user);
            Assertions.assertEquals(delCnt, 0);
        }
    }

    @Test
    public void testUpdateUser() {
        Long updateUserId = 10001L;
        String updateUserName = "test_user2";

        // successful update user by id
        {
            User user = new User();
            user.setId(updateUserId);

            User updateUser = new User();
            updateUser.setName("test_user1_rename");
            updateUser.setStatus(1);
            int updateCnt = userMapper.updateUser(user, updateUser);
            Assertions.assertEquals(updateCnt, 1);

            List<User> users = userMapper.queryUser(user);
            Assertions.assertEquals(users.size(), 1);
            Assertions.assertEquals(users.get(0).getId(), user.getId());
            Assertions.assertEquals(users.get(0).getName(), updateUser.getName());
            Assertions.assertEquals(users.get(0).getStatus(), updateUser.getStatus());
        }

        // successful update user by name
        {
            User user = new User();
            user.setName(updateUserName);

            User updateUser = new User();
            updateUser.setName("test_user2_rename");
            updateUser.setStatus(0);
            int updateCnt = userMapper.updateUser(user, updateUser);
            Assertions.assertEquals(updateCnt, 1);

            List<User> users = userMapper.queryUser(updateUser);
            Assertions.assertEquals(users.size(), 1);
            Assertions.assertEquals(users.get(0).getName(), updateUser.getName());
            Assertions.assertEquals(users.get(0).getStatus(), updateUser.getStatus());
        }
    }

    @Test
    public void testUpdateUserPassword() {
        Long userId = 10001L;
        String userName = "test_user2";

        // successful update user by id
        {
            User user = new User();
            user.setId(userId);
            user.setPassword("xyz123456");
            int updateCnt = userMapper.updateUserPassword(user);
            Assertions.assertEquals(updateCnt, 1);

            List<User> users = userMapper.queryUserPassword(user);
            Assertions.assertEquals(users.size(), 1);
            Assertions.assertEquals(users.get(0).getPassword(), user.getPassword());
        }

        // successful update user by name
        {
            User user = new User();
            user.setName(userName);
            user.setPassword("xyz123456");
            int updateCnt = userMapper.updateUserPassword(user);
            Assertions.assertEquals(updateCnt, 1);

            List<User> users = userMapper.queryUserPassword(user);
            Assertions.assertEquals(users.size(), 1);
            Assertions.assertEquals(users.get(0).getPassword(), user.getPassword());
        }
    }

}
