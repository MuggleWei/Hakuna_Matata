package com.muggle.app.mapper;

import com.muggle.app.model.City;
import org.apache.ibatis.annotations.*;

import java.util.List;

public interface CityMapper {
    /*
    * select all city
    * */
    @Select({"select * from city"})
    @Results(id = "cityResultMap", value = {
            @Result(property = "id", column = "ID")
    })
    List<City> selectAll();

    /*
    * select city by name
    * */
    @ResultMap("cityResultMap")
    @Select({"select * from city where CountryCode = #{countryCode}"})
    List<City> selectByCountryCode(String countryCode);
}
