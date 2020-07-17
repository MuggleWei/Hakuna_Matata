package com.muggle.dcuser.mapper;

import com.muggle.dcuser.entity.Authority;
import org.apache.ibatis.annotations.Mapper;
import org.apache.ibatis.annotations.Param;

import java.util.List;

@Mapper
public interface AuthorityMapper {
    // query authority
    List<Authority> queryAuthority(@Param("user_id") Long userId);

    // add authority
    int addAuthority(@Param("authority") Authority authority);

    // del authority
    int delAuthority(@Param("user_id") Long userId);

    // update authority
    int updateAuthority(@Param("authority") Authority authority);
}
