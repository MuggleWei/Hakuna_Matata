package com.muggle.service;

import com.muggle.entity.Level;
import com.muggle.entity.User;
import com.muggle.mapper.LevelMapper;
import com.muggle.mapper.UserMapper;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.jdbc.datasource.DataSourceTransactionManager;
import org.springframework.stereotype.Service;
import org.springframework.transaction.TransactionStatus;
import org.springframework.transaction.annotation.Transactional;
import org.springframework.transaction.support.DefaultTransactionDefinition;

import java.util.List;

@Service
public class UserService {
    @Autowired
    private UserMapper userMapper;

    @Autowired
    private LevelMapper levelMapper;

    @Autowired
    private DataSourceTransactionManager transactionManager;

    public User getUserById(Long id) {
        User user = new User();
        user.setId(id);
        List<User> users = userMapper.queryUser(user);
        if (users.size() == 1) {
            return users.get(0);
        }
        return null;
    }

    public Level getUserLevel(Long id) {
        List<Level> levels = levelMapper.queryLevel(id);
        if (levels.size() == 1) {
            return levels.get(0);
        }
        return null;
    }

    @Transactional(rollbackFor = Exception.class)
    public int addUser(User user, Level level) {
        int addCnt = 0;
        addCnt = userMapper.addUser(user);
        if (addCnt != 1) {
            throw new RuntimeException("failed add user");
        }

        addCnt = levelMapper.addLevel(level);
        if (addCnt != 1) {
            throw new RuntimeException("failed add level");
        }

        return addCnt;
    }
}
