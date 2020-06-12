package com.muggle.use.mybatis.mapper;

import com.muggle.use.mybatis.model.User;
import org.apache.ibatis.annotations.Param;

import java.sql.Timestamp;
import java.util.List;

public interface UserMapper {

    /**
     * query all users
     */
    List<User> selectAll();

    /**
     * query user by user id
     */
    List<User> findUserById(@Param("userId") long userId);

    /**
     * find user by name and register ip
     */
    List<User> findUser(
            @Param("userName") String userName,
            @Param("regIp") String regIp);

    /**
     * insert user
     */
    int insertUser(User user);

    /**
     * insert user list
     */
    int insertUserArray(List<User> users);

    /**
     * update user
     */
    int updateUser(
            @Param("userId") Long userId,
            @Param("userName") String userName,
            @Param("regDate") Timestamp regDate,
            @Param("regIp") String regIp);

    /**
     * delete user
     */
    int deleteUser(
            @Param("userId") Long userId,
            @Param("userName") String userName);
}
