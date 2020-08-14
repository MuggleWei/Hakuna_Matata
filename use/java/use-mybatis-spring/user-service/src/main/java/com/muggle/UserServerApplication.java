package com.muggle;

import com.muggle.entity.User;
import com.muggle.service.UserService;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.context.ApplicationContext;
import org.springframework.context.support.ClassPathXmlApplicationContext;

public class UserServerApplication {
    private static final Logger logger = LoggerFactory.getLogger(UserServerApplication.class);

    public static void main(String[] args) {
        System.setProperty("spring.profiles.default", "dev");

        ApplicationContext context = new ClassPathXmlApplicationContext("application.xml");
        UserService userService = context.getBean(UserService.class);
        User user = userService.getUserById(10001L);
        if (user != null) {
            logger.info(user.getName());
        } else {
            logger.warn("can't find user");
        }
    }
}
