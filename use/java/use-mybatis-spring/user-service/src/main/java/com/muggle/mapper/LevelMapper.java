package com.muggle.mapper;

import com.muggle.entity.Level;
import org.apache.ibatis.annotations.Insert;
import org.apache.ibatis.annotations.Mapper;
import org.apache.ibatis.annotations.Param;
import org.apache.ibatis.annotations.Select;

import java.util.List;

@Mapper
public interface LevelMapper {

    /**
     * @param id user id
     * @return level list
     */
    @Select("select id, level from t_level where id = #{id}")
    List<Level> queryLevel(@Param("id") Long id);

    /**
     * @param level level
     * @return add level count
     */
    @Insert("insert into t_level (id, level) values (#{level.id}, #{level.level})")
    int addLevel(@Param("level") Level level);
}
