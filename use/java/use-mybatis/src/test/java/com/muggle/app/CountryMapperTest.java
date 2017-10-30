package com.muggle.app;

import com.muggle.app.mapper.CountryMapper;
import com.muggle.app.model.Country;
import org.apache.ibatis.session.SqlSession;
import org.junit.Assert;
import org.junit.FixMethodOrder;
import org.junit.Test;
import org.junit.runners.MethodSorters;

import java.util.HashSet;
import java.util.List;
import java.util.Set;

/**
 * Created by mugglewei on 2017/10/28.
 */
@FixMethodOrder(MethodSorters.JVM) // test by method definition order
public class CountryMapperTest extends BaseMapperTest {
    @Test
    public void testSelectAll() {
        SqlSession sqlSession = getSqlSession();
        try {
            CountryMapper countryMapper = sqlSession.getMapper(CountryMapper.class);
            List<Country> countryList = countryMapper.selectAll();
            Assert.assertTrue(countryList.size() > 0);
            for (Country country : countryList) {
                Assert.assertNotNull(country);
                Assert.assertNotNull(country.getCode());
            }
        } finally {
            sqlSession.close();
        }
    }


    @Test
    public void testSelectCodes() {
        SqlSession sqlSession = getSqlSession();
        try {
            CountryMapper countryMapper = sqlSession.getMapper(CountryMapper.class);
            List<String> codes = countryMapper.selectCodes();
            Assert.assertTrue(codes.size() > 0);
            for (String code : codes) {
                Assert.assertNotNull(code);
            }
        } finally {
            sqlSession.close();
        }
    }

    @Test
    public void testSelectRegions() {
        SqlSession sqlSession = getSqlSession();
        try {
            CountryMapper countryMapper = sqlSession.getMapper(CountryMapper.class);
            List<String> regions = countryMapper.selectRegions();
            Assert.assertTrue(regions.size() > 0);

            Set<String> regionSet = new HashSet<String>();
            for (String region : regions) {
                Assert.assertNotNull(region);
                Assert.assertFalse(regionSet.contains(region));
                regionSet.add(region);
            }
        } finally {
            sqlSession.close();
        }
    }

    @Test
    public void testSelectByContinent() {
        SqlSession sqlSession = getSqlSession();
        try {
            CountryMapper countryMapper = sqlSession.getMapper(CountryMapper.class);
            List<Country> countryList = countryMapper.selectByContinent("Asia");
            Assert.assertTrue(countryList.size() > 0);

            for (Country country : countryList) {
                Assert.assertNotNull(country);
                Assert.assertEquals(country.getContinent(), "Asia");
            }
        } finally {
            sqlSession.close();
        }
    }

    @Test
    public void testInsertCountry() {
        SqlSession sqlSession = getSqlSession();
        try {
            CountryMapper countryMapper = sqlSession.getMapper(CountryMapper.class);
            Country country = new Country();
            country.setCode("WSZ");
            country.setName("muggle");
            country.setContinent("Asia");
            int result = countryMapper.insertCountry(country);
            Assert.assertEquals(1, result);
            sqlSession.commit();
        } finally {
            sqlSession.close();
        }
    }

    @Test
    public void testUpdateCountry() {
        SqlSession sqlSession = getSqlSession();
        try {
            CountryMapper countryMapper = sqlSession.getMapper(CountryMapper.class);
            int result = countryMapper.updateByCode("mugglewei", "WSZ");
            Assert.assertEquals(1, result);
            sqlSession.commit();
        } finally {
            sqlSession.close();
        }
    }

    @Test
    public void testDeleteCountry() {
        SqlSession sqlSession = getSqlSession();
        try {
            CountryMapper countryMapper = sqlSession.getMapper(CountryMapper.class);
            int result = countryMapper.deleteByName("mugglewei");
            Assert.assertEquals(1, result);
            sqlSession.commit();
        } finally {
            sqlSession.close();
        }
    }

}
