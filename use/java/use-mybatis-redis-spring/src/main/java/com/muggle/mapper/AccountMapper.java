package com.muggle.mapper;

import com.muggle.model.Account;
import org.apache.ibatis.annotations.Options;
import org.apache.ibatis.annotations.Select;
import org.apache.ibatis.annotations.Update;

import java.util.List;

public interface AccountMapper {

    @Select({"select * from account_info"})
    List<Account> selectAll();

    @Update({"insert into account_info (name, phone, mail) value (#{name}, #{phone}, #{mail})"})
    @Options(useGeneratedKeys=true, keyProperty="id")
    int createAccountInfo(Account account) throws Exception;

    @Update({"insert into account_passwd (id, passwd) value (#{id}, #{password})"})
    int createAccountPasswd(Account account) throws Exception;

    @Select({"select count(*) from account_info"})
    int getAccountInfoCnt();

    @Select({"select count(*) from account_passwd"})
    int getAccountPasswdCnt();

    @Select({"select * from account_info where id = #{id}"})
    List<Account> selectByID(long id);
}
