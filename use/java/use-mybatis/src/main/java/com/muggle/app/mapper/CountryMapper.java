package com.muggle.app.mapper;

import com.muggle.app.model.Country;
import org.apache.ibatis.annotations.Param;

import java.util.List;

/**
 * Created by mugglewei on 2017/10/28.
 */
public interface CountryMapper {
    /*
    * select all country
    * */
    List<Country> selectAll();

    /*
    * get all country code
    * */
    List<String> selectCodes();

    /*
    * get all region
    * */
    List<String> selectRegions();

    /*
    * select country by Continent
    * */
    List<Country> selectByContinent(@Param("continent") String continent);

    /*
    * insert country
    * */
    int insertCountry(Country country);

    /*
    * update country name by code
    * */
    int updateByCode(@Param("name") String newName, @Param("code") String code);

    /*
    * delete country by name
    * */
    int deleteByName(@Param("name") String name);

}
