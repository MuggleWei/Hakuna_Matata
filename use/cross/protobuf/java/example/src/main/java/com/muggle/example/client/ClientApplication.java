package com.muggle.example.client;

import com.muggle.example.client.config.ClientConfig;
import org.springframework.context.ApplicationContext;
import org.springframework.context.annotation.AnnotationConfigApplicationContext;

public class ClientApplication {
    public static void main(String[] args) throws Exception {
        ApplicationContext ctx = new AnnotationConfigApplicationContext(
                ClientConfig.class
        );

        Client client = ctx.getBean(Client.class);
        client.run();
    }
}
