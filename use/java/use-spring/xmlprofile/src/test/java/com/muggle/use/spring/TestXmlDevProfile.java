package com.muggle.use.spring;

import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.extension.ExtendWith;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.test.context.ActiveProfiles;
import org.springframework.test.context.ContextConfiguration;
import org.springframework.test.context.junit.jupiter.SpringExtension;

import java.util.Properties;

import static org.junit.jupiter.api.Assertions.assertEquals;

@ExtendWith(SpringExtension.class)
@ContextConfiguration({"classpath:applicationContext.xml"})
@ActiveProfiles("dev")
public class TestXmlDevProfile {

    @Autowired
    private MyDataSourceProperties dataSourceProperties;

    @Test
    public void isDev() {
        assertEquals(dataSourceProperties.getConfigName(), "dev");
    }
}
