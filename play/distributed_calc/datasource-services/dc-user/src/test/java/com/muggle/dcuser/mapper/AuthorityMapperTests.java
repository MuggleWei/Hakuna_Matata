package com.muggle.dcuser.mapper;

import com.muggle.dcuser.entity.Authority;
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
public class AuthorityMapperTests {
    private final Logger logger = LoggerFactory.getLogger(this.getClass());

    @Autowired
    private AuthorityMapper authorityMapper;

    @Test
    public void testQueryAuthority() {
        Long userId = 10001L;
        Integer numTasks = 1;

        // query by id
        {
            List<Authority> authorityList = authorityMapper.queryAuthority(userId);
            Assertions.assertEquals(authorityList.size(), 1);
            Assertions.assertEquals(authorityList.get(0).getUserId(), userId);
            Assertions.assertEquals(authorityList.get(0).getNumTasks(), numTasks);
        }

        // query user not exists id
        {
            List<Authority> authorityList = authorityMapper.queryAuthority(10010L);
            Assertions.assertEquals(authorityList.size(), 0);
        }
    }

    @Test
    public void testAddAuthority() {
        Long userId = 10005L;
        Integer numTasks = 5;

        // successful add authority
        {
            Authority authority = new Authority();
            authority.setUserId(userId);
            authority.setNumTasks(numTasks);
            int addCnt = authorityMapper.addAuthority(authority);
            Assertions.assertEquals(addCnt, 1);
        }

        // failed add authority, repeated user id
        {
            Authority authority = new Authority();
            authority.setUserId(userId);
            authority.setNumTasks(numTasks);
            boolean catchException = false;
            try {
                int addCnt = authorityMapper.addAuthority(authority);
            } catch (Exception e) {
                catchException = true;
            }
            Assertions.assertTrue(catchException);
        }
    }

    @Test
    public void testDelAuthority() {
        Long userId = 10001L;

        // successful delete user by id
        {
            int delCnt = authorityMapper.delAuthority(userId);
            Assertions.assertEquals(delCnt, 1);
        }

        // failed delete user by id
        {
            int delCnt = authorityMapper.delAuthority(10010L);
            Assertions.assertEquals(delCnt, 0);
        }
    }

    @Test
    public void testUpdateAuthority() {
        Long userId = 10001L;
        Integer numTasks = 100;

        // successful update authority
        {
            Authority authority = new Authority();
            authority.setUserId(userId);
            authority.setNumTasks(numTasks);
            int updateCnt = authorityMapper.updateAuthority(authority);
            Assertions.assertEquals(updateCnt, 1);

            List<Authority> authorityList = authorityMapper.queryAuthority(userId);
            Assertions.assertEquals(authorityList.size(), 1);
            Assertions.assertEquals(authorityList.get(0).getUserId(), userId);
            Assertions.assertEquals(authorityList.get(0).getNumTasks(), numTasks);
        }
    }
}
