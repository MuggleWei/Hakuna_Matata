package com.muggle.dcuser.service;

import com.muggle.common.utils.ErrorIdUtils;
import com.muggle.dcuser.componenet.PasswordUtils;
import com.muggle.dcuser.mapper.AuthorityMapper;
import com.muggle.dcuser.mapper.UserMapper;
import com.muggle.dcuser.model.Authority;
import com.muggle.dcuser.model.User;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Propagation;
import org.springframework.transaction.annotation.Transactional;

import java.security.NoSuchAlgorithmException;
import java.util.List;
import java.util.Objects;

@Service
public class UserService {
    private final Logger logger = LoggerFactory.getLogger(this.getClass());

    @Autowired
    private UserMapper userMapper;

    @Autowired
    private AuthorityMapper authorityMapper;

    @Autowired
    private PasswordUtils passwordUtils;

    @Autowired
    private ErrorIdUtils errorIdUtils;

    public User queryUser(User user) {
        List<User> userList = userMapper.queryUser(user);
        if (userList.size() != 1) {
            logger.warn("failed query user: " + user);
            return null;
        }

        return userList.get(0);
    }

    public Authority queryUserAuthority(Long userId) {
        List<Authority> authorityList = authorityMapper.queryAuthority(userId);
        if (authorityList.size() != 1) {
            logger.warn("failed query authority: " + userId);
            return null;
        }

        return authorityList.get(0);
    }

    public int checkUserPassword(User user) {
        List<User> userList = userMapper.queryUserPassword(user);
        if (userList.size() != 1) {
            logger.warn("failed query user: " + user);
            return errorIdUtils.errIdInvalidUser;
        }

        try {
            if (passwordUtils.bcryptCheck(user.getPassword(), userList.get(0).getPassword())) {
                return errorIdUtils.ok;
            }
        } catch (NoSuchAlgorithmException e) {
            logger.warn(e.getMessage(), e);
        }

        return errorIdUtils.errIdPassword;
    }

    public int addUser(User user) {
        int ret = 0;
        try {
            ret = realAddUser(user);
            user.setPassword(null);
            if (ret != errorIdUtils.ok) {
                logger.warn("failed add user " + user);
            }
        } catch (Exception e) {
            user.setPassword(null);
            logger.warn(e.getMessage(), e);
            return -1;
        }
        return ret;
    }

    public int delUser(User user) {
        int ret = 0;
        try {
            ret = realDelUser(user);
            if (ret != errorIdUtils.ok) {
                logger.warn("failed del user " + user);
            }
        } catch (Exception e) {
            logger.warn(e.getMessage(), e);
            return -1;
        }
        return ret;
    }

    public int updateUser(User user, User updateUser) {
        try {
            int updateCnt = userMapper.updateUser(user, updateUser);
            if (updateCnt != 1) {
                logger.warn("failed update user, user: " + user + ", update info: " + updateUser);
                return -1;
            }
        } catch (Exception e) {
            logger.warn(e.getMessage(), e);
            return -1;
        }

        return errorIdUtils.ok;
    }

    public int updateAuthority(Authority authority) {
        if (authority.getUserId() == null) {
            return errorIdUtils.errIdInvalidUser;
        }

        try {
            int updateCnt = authorityMapper.updateAuthority(authority);
            if (updateCnt != 1) {
                return -1;
            }
        } catch (Exception e) {
            logger.warn(e.getMessage(), e);
            return -1;
        }

        return errorIdUtils.ok;
    }

    @Transactional(propagation = Propagation.REQUIRED, rollbackFor = Exception.class)
    public int realAddUser(User user) {
        if (user.getName() == null || user.getName().length() > 32 || user.getName().length() == 0) {
            return errorIdUtils.errIdInvalidUser;
        }

        if (user.getPassword() == null || user.getPassword().length() > 32 || user.getPassword().length() < 8) {
            return errorIdUtils.errIdPasswordLen;
        }

        // generate bcrypt hash
        String hashed = "";
        try {
            hashed = passwordUtils.bcryptHash(user.getPassword());
        } catch (NoSuchAlgorithmException e) {
            logger.error(e.getMessage(), e);
            return -1;
        }
        user.setPassword(hashed);

        // add user
        try {
            int addCnt = userMapper.addUser(user);
            if (addCnt != 1) {
                logger.warn("failed add user: " + user);
                return -1;
            }
        } catch (Exception e) {
            logger.warn(e.toString());
            return errorIdUtils.errIdRepeatedUserName;
        }

        // add authority
        Authority authority = new Authority();
        authority.setUserId(user.getId());
        authority.setNumTasks(1);

        try {
            authorityMapper.addAuthority(authority);
        } catch (Exception e) {
            logger.error(e.getMessage(), e);
            throw e;
        }

        return 0;
    }

    @Transactional(rollbackFor = Exception.class)
    public int realDelUser(User user) {
        List<User> userList = userMapper.queryUser(user);
        if (userList.size() != 1) {
            return errorIdUtils.errIdInvalidUser;
        }

        user = userList.get(0);
        Long userId = user.getId();
        int delCnt = userMapper.delUser(user);
        if (delCnt != 1) {
            String errMsg = "failed delete user " + user.getId();
            throw new RuntimeException(errMsg);
        }

        delCnt = authorityMapper.delAuthority(userId);
        if (delCnt != 1) {
            String errMsg = "failed delete authority " + userId;
            throw new RuntimeException(errMsg);
        }

        return errorIdUtils.ok;
    }
}
