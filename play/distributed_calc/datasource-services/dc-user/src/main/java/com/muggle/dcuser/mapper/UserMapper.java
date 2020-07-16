package com.muggle.dcuser.mapper;

import com.muggle.dcuser.model.User;
import org.apache.ibatis.annotations.Mapper;
import org.apache.ibatis.annotations.Param;

import java.util.List;

@Mapper
public interface UserMapper {
    // query user
    List<User> queryUser(@Param("user") User user);

    // query user password
    List<User> queryUserPassword(@Param("user") User user);

    // add new user
    int addUser(@Param("user") User user);

    // delete user
    int delUser(@Param("user") User user);

    // update user
    int updateUser(@Param("user") User user, @Param("new_user") User newUser);

    // update user password
    int updateUserPassword(@Param("user") User user);
}
