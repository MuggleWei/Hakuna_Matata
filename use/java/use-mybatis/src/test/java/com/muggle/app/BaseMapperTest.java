package com.muggle.app;

import org.apache.ibatis.io.Resources;
import org.apache.ibatis.session.SqlSession;
import org.apache.ibatis.session.SqlSessionFactory;
import org.apache.ibatis.session.SqlSessionFactoryBuilder;
import org.junit.BeforeClass;

import java.io.FileInputStream;
import java.io.IOException;
import java.io.Reader;
import java.util.Properties;

/**
 * Created by mugglewei on 2017/10/28.
 */
public class BaseMapperTest {
    private static SqlSessionFactory sqlSessionFactory = null;

    @BeforeClass
    public static void init() {
        try (Reader reader = Resources.getResourceAsReader("mybatis-config.xml")){
            Properties prop = new Properties();
            prop.load(new FileInputStream("config/db.properties"));
            sqlSessionFactory = new SqlSessionFactoryBuilder().build(reader, prop);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public SqlSession getSqlSession() {
        return sqlSessionFactory.openSession();
    }
}
