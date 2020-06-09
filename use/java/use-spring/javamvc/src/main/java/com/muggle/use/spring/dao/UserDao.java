package com.muggle.use.spring.dao;

import com.muggle.use.spring.domain.User;

import java.util.List;

public interface UserDao {
    List<User> getUsers(int startIdx, int count);

    User findUserById(int userId);

    User findUserByName(String userName);

    int addUser(String userName, String password, String phone, String email);
}
