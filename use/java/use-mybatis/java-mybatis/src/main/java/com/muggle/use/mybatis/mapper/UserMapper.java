package com.muggle.use.mybatis.mapper;

import com.muggle.use.mybatis.model.User;
import org.apache.ibatis.annotations.ResultMap;
import org.apache.ibatis.annotations.Results;
import org.apache.ibatis.annotations.Select;

import java.util.List;

public interface UserMapper {
    @Select({"select * from t_user"})
    @Results(id = "userMapper", value = {
            // don't need map underscore to camel case manual, set mapUnderscoreToCamelCase in mybatis-config.xml
            // @Result(property = "userId", column = "user_id")
    })
    List<User> selectAll();

    @ResultMap("userMapper")
    @Select({"select * from t_user where user_id=#{userId}"})
    List<User> selectById(long userId);
}
