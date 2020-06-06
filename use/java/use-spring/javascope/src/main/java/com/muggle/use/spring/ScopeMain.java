package com.muggle.use.spring;

import com.muggle.use.spring.config.ScopeConfig;
import org.springframework.context.annotation.AnnotationConfigApplicationContext;

public class ScopeMain {
    public static void main(String[] args) {
        AnnotationConfigApplicationContext context = new AnnotationConfigApplicationContext(ScopeConfig.class);

        for (int i = 0; i < 10; i++) {
            CalcTask task = context.getBean(CalcTask.class);
            task.run();
        }

        context.close();
    }
}
