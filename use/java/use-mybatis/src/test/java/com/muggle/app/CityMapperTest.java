package com.muggle.app;

import com.muggle.app.mapper.CityMapper;
import com.muggle.app.model.City;
import org.apache.ibatis.session.SqlSession;
import org.junit.Assert;
import org.junit.Test;

import java.util.List;

public class CityMapperTest extends BaseMapperTest {
    @Test
    public void testSelectAll() {
        try (SqlSession sqlSession = getSqlSession()){
            CityMapper cityMapper = sqlSession.getMapper(CityMapper.class);
            List<City> cityList = cityMapper.selectAll();
            Assert.assertTrue(cityList.size() > 0);
            for (City city : cityList) {
                Assert.assertNotNull(city);
                Assert.assertNotNull(city.getId());
            }
        }
    }

    @Test
    public void testSelectByCountryCode() {
        try (SqlSession sqlSession = getSqlSession()) {
            CityMapper cityMapper = sqlSession.getMapper(CityMapper.class);
            List<City> cityList = cityMapper.selectByCountryCode("CHN");
            Assert.assertTrue(cityList.size() > 0);
            for (City city : cityList) {
                Assert.assertNotNull(city);
                Assert.assertNotNull(city.getId());
                Assert.assertEquals(city.getCountryCode(), "CHN");
            }
        }
    }
}
