package com.muggle.use.spring.service;

import com.muggle.use.spring.dao.UserDao;
import com.muggle.use.spring.domain.RegisterInfo;
import com.muggle.use.spring.domain.User;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.List;
import java.util.Objects;

@Service
public class UserService {
    private UserDao userDao;

    @Autowired
    public void setUserDao(UserDao userDao) {
        this.userDao = userDao;
    }

    public User findUserByName(String userName) {
        return userDao.findUserByName(userName);
    }

    public User findUserById(int userId) {
        return userDao.findUserById(userId);
    }

    public List<User> getUsers(int pageIndex, int usersPerPage) {
        return userDao.getUsers((pageIndex - 1) * usersPerPage, usersPerPage);
    }

    public int addUser(RegisterInfo registerInfo) {
        return userDao.addUser(registerInfo.getUserName(), registerInfo.getPassWord(), registerInfo.getPhoneNumber(), registerInfo.getEmail());
    }

    public boolean hasMatchUser(String userName, String password) {
        User user = findUserByName(userName);
        if (user == null) {
            return false;
        }

        return Objects.equals(user.getPassWord(), password);
    }
}
