package com.muggle.use.druid;

import com.alibaba.druid.pool.DruidDataSourceFactory;

import javax.sql.DataSource;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.sql.Connection;
import java.util.Properties;

public class ExampleDruid {
    public static void main(String[] args) throws IOException {
        Properties properties = getDruidProperties();

        DataSource dataSource = null;
        try {
            dataSource = DruidDataSourceFactory.createDataSource(properties);
            Connection conn = dataSource.getConnection();

            // Do something

            conn.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static Properties getDruidProperties() throws IOException {
        Properties properties = new Properties();

        // e.g. -Ddruid.configurationFile=druid.properties
        String dcbpPropertiesFile = System.getProperty("druid.configurationFile");
        if (dcbpPropertiesFile != null) {
            properties.load(new FileReader(dcbpPropertiesFile));
        } else {
            InputStream inputStream = ExampleDruid.class.getClassLoader().getResourceAsStream("druid.properties");
            properties.load(inputStream);
        }
        return properties;
    }
}
