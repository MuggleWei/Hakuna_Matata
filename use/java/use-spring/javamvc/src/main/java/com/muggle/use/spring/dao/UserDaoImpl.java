package com.muggle.use.spring.dao;

import com.muggle.use.spring.domain.User;
import org.springframework.stereotype.Repository;

import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

/**
 * fake jdbc user dao
 */
@Repository
public class UserDaoImpl implements UserDao {
    private ArrayList<User> users;

    public UserDaoImpl() {
        users = new ArrayList<>();
        for (int i = 0; i < 100; i++) {
            users.add(new User(i, "u" + i, "123456", "", ""));
        }
    }


    @Override
    public List<User> getUsers(int startIdx, int count) {
        if (startIdx < 0 || count < 1 || startIdx >= users.size()) {
            return new ArrayList<>();
        }

        int endIdx = startIdx + count;
        if (endIdx > users.size()) {
            endIdx = users.size();
        }

        ArrayList<User> retList = new ArrayList<>();
        for (int i = startIdx; i < endIdx; i++) {
            retList.add(users.get(i));
        }

        return retList;
    }

    @Override
    public User findUserById(int userId) {
        if (userId < 0 || userId >= users.size()) {
            return null;
        }

        return users.get(userId);
    }

    @Override
    public User findUserByName(String userName) {
        for (User user : users) {
            if (Objects.equals(user.getUserName(), userName)) {
                return user;
            }
        }
        return null;
    }

    @Override
    public int addUser(String userName, String password, String phone, String email) {
        int userId = 0;
        synchronized (this) {
            userId = users.size();
            users.add(new User(userId, "u" + userId, password, phone, email));
        }
        return userId;
    }
}
