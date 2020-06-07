package com.muggle.use.spring.task;

import org.springframework.stereotype.Component;

@Component
public class NormalTask implements Task {
    @Override
    public void run() {
        System.out.println("run normal task");
        try {
            Thread.sleep(500);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
}
