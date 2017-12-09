package com.muggle.example.server.gate;

import com.muggle.example.server.gate.config.GateConfig;
import org.springframework.context.ApplicationContext;
import org.springframework.context.annotation.AnnotationConfigApplicationContext;

public class GateApplication {
    public static void main(String[] args) throws Exception {
        ApplicationContext ctx = new AnnotationConfigApplicationContext(
                GateConfig.class
        );

        Gate gate = ctx.getBean(Gate.class);
        gate.run();
    }
}
