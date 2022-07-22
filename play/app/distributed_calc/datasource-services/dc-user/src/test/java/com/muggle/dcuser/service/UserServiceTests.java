package com.muggle.dcuser.service;

import com.muggle.common.utils.ErrorIdUtils;
import com.muggle.dcuser.mapper.AuthorityMapper;
import com.muggle.dcuser.entity.Authority;
import com.muggle.dcuser.entity.User;
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
    public void testAddUserSuccess() {
        String userName = "test_user";
        String plainPassword = "xyz123456abc";

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
        }
    }

    @Test
    public void testAddUserFailedTransaction() {
        String userName = "test_user";
        String plainPassword = "xyz123456abc";
        Long userId = 10003L;

        // failed add user cause duplicated authority key user_id
        {
            Authority authority = new Authority();
            authority.setUserId(userId);
            authority.setNumTasks(1);
            int ret = authorityMapper.addAuthority(authority);
            Assertions.assertEquals(ret, 1);

            User user = new User();
            user.setName(userName);
            user.setPassword(plainPassword);
            boolean catchException = false;
            try {
                ret = userService.addUser(user);
                Assertions.assertTrue(ret != errorIdUtils.ok);
            } catch (Exception e) {
                catchException = true;
            }
            Assertions.assertTrue(catchException);

            user.setId(null);
            User rspUser = userService.queryUser(user);
            Assertions.assertNull(rspUser);
        }
    }

    @Test
    public void testDelUserSuccess() {
        Long userId1 = 10001L;
        String userName2 = "test_user2";

        // delete by user id
        {
            User user = new User();
            user.setId(userId1);
            int ret = userService.delUser(user);
            Assertions.assertEquals(ret, errorIdUtils.ok);
        }

        // delete by user name
        {
            User user = new User();
            user.setName(userName2);
            int ret = userService.delUser(user);
            Assertions.assertEquals(ret, errorIdUtils.ok);
        }
    }

    @Test
    public void testDelUserFailed() {
        // delete not exists user id
        {
            User user = new User();
            user.setId(10010L);
            int ret = userService.delUser(user);
            Assertions.assertNotEquals(ret, errorIdUtils.ok);
        }

        // delete not exists user name
        {
            User user = new User();
            user.setName("test_user_not_exists");
            int ret = userService.delUser(user);
            Assertions.assertNotEquals(ret, errorIdUtils.ok);
        }

        // delete failed cause transcation
        {
            int delCnt = authorityMapper.delAuthority(10001L);
            Assertions.assertEquals(delCnt, 1);

            User user = new User();
            user.setId(10001L);
            boolean catchException = false;
            try {
                userService.delUser(user);
            } catch (Exception e) {
                catchException = true;
            }
            Assertions.assertTrue(catchException);

            User rspUser = userService.queryUser(user);
            Assertions.assertNotNull(rspUser);
            Assertions.assertEquals(rspUser.getId(), 10001L);
        }
    }

    @Test
    public void testUpdateUser() {
        // update user by user id
        {
            User user = new User();
            User updateUser = new User();

            user.setId(10001L);
            updateUser.setName("test_user1_update_name");
            updateUser.setStatus(1);

            int ret = userService.updateUser(user, updateUser);
            Assertions.assertEquals(ret, errorIdUtils.ok);

            User rspUser = userService.queryUser(user);
            Assertions.assertEquals(rspUser.getId(), user.getId());
            Assertions.assertEquals(rspUser.getName(), updateUser.getName());
            Assertions.assertEquals(rspUser.getStatus(), updateUser.getStatus());
        }

        // update user by user name
        {
            User user = new User();
            User updateUser = new User();

            user.setName("test_user2");
            updateUser.setName("test_user2_update_name");
            updateUser.setStatus(0);

            int ret = userService.updateUser(user, updateUser);
            Assertions.assertEquals(ret, errorIdUtils.ok);

            User rspUser = userService.queryUser(updateUser);
            Assertions.assertEquals(rspUser.getName(), updateUser.getName());
            Assertions.assertEquals(rspUser.getStatus(), updateUser.getStatus());
        }
    }

    @Test
    public void testUpdatePassword() {
        // update password by user id
        {
            User user = new User();
            user.setId(10001L);

            String newPassword = "newpassword123";
            int ret = userService.updatePassword(user, newPassword);
            Assertions.assertEquals(ret, errorIdUtils.ok);

            User rspUser = userService.queryUser(user);
            Assertions.assertEquals(rspUser.getId(), user.getId());

            user.setPassword(newPassword);
            ret = userService.checkUserPassword(user);
            Assertions.assertEquals(ret, errorIdUtils.ok);
        }

        // update password by user name
        {
            User user = new User();
            user.setName("test_user2");

            String newPassword = "newpassword123";
            int ret = userService.updatePassword(user, newPassword);
            Assertions.assertEquals(ret, errorIdUtils.ok);

            User rspUser = userService.queryUser(user);
            Assertions.assertEquals(rspUser.getName(), user.getName());

            user.setPassword(newPassword);
            ret = userService.checkUserPassword(user);
            Assertions.assertEquals(ret, errorIdUtils.ok);
        }
    }

    @Test
    public void testUpdateAuthority() {
        Authority authority = new Authority();
        authority.setUserId(10001L);
        authority.setNumTasks(15);

        int ret = userService.updateAuthority(authority);
        Assertions.assertEquals(ret, errorIdUtils.ok);

        authority = userService.queryUserAuthority(10001L);
        Assertions.assertEquals(authority.getNumTasks(), 15);
    }

}
