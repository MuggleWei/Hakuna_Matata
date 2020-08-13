package com.muggle.mapper;

import com.muggle.entity.User;
import org.apache.ibatis.annotations.Mapper;
import org.apache.ibatis.annotations.Param;

import java.util.List;

@Mapper
public interface UserMapper {

    /**
     * query user by id or name
     *
     * @param user user contain id or name
     * @return user list
     */
    List<User> queryUser(@Param("user") User user);

    /**
     * add new user
     *
     * @param user new user
     * @return add user count
     */
    int addUser(@Param("user") User user);

    /**
     * delete user by id or name
     *
     * @param user user contain id or name
     * @return delete user count
     */
    int delUser(@Param("user") User user);

    /**
     * update user
     *
     * @param id user id
     * @param name user name
     * @param user update user info
     * @return update user count
     */
    int updateUser(
            @Param("id") Long id,
            @Param("name") String name,
            @Param("user") User user);

}
