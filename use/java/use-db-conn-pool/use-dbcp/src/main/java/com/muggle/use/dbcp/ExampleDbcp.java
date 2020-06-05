package com.muggle.use.dbcp;

import org.apache.commons.dbcp2.BasicDataSourceFactory;

import javax.sql.DataSource;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.sql.Connection;
import java.util.Properties;

public class ExampleDbcp {
    public static void main(String[] args) throws IOException {
        Properties properties = getDcbpProperties();

        DataSource dataSource = null;
        try {
            dataSource = BasicDataSourceFactory.createDataSource(properties);
            Connection conn = dataSource.getConnection();

            // Do something

            conn.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static Properties getDcbpProperties() throws IOException {
        Properties properties = new Properties();

        // e.g. -Ddcbp.configurationFile=dbcp.properties
        String dcbpPropertiesFile = System.getProperty("dcbp.configurationFile");
        if (dcbpPropertiesFile != null) {
            properties.load(new FileReader(dcbpPropertiesFile));
        } else {
            InputStream inputStream = ExampleDbcp.class.getClassLoader().getResourceAsStream("dbcp.properties");
            properties.load(inputStream);
        }
        return properties;
    }
}
