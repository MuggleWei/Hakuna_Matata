package com.muggle.use.spring.task;

import org.springframework.stereotype.Component;

@Component
public class BadTask implements Task {
    @Override
    public void run() throws RuntimeException {
        System.out.println("run bad task");
        throw new RuntimeException("bad task throw exception");
    }
}
