package com.muggle.use.mybatis;

import org.apache.ibatis.io.Resources;
import org.apache.ibatis.session.SqlSession;
import org.apache.ibatis.session.SqlSessionFactory;
import org.apache.ibatis.session.SqlSessionFactoryBuilder;
import org.junit.jupiter.api.BeforeAll;

import java.io.IOException;
import java.io.Reader;

public class TestBaseMapper {
    private static SqlSessionFactory sqlSessionFactory = null;

    @BeforeAll
    public static void init() {
        try (Reader reader = Resources.getResourceAsReader("mybatis-config.xml")) {
//            Properties prop = new Properties();
//            prop.load(new FileInputStream("config/db.properties"));
//            sqlSessionFactory = new SqlSessionFactoryBuilder().build(reader, prop);

            // set db properties in mybatis-config.xml configuration/properties
            sqlSessionFactory = new SqlSessionFactoryBuilder().build(reader);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public SqlSession getSqlSession() {
        return sqlSessionFactory.openSession();
    }
}
