package com.muggle.use.spring;

import org.springframework.stereotype.Component;

import java.util.concurrent.atomic.AtomicInteger;

@Component
public class IdAlloc {
    private AtomicInteger id = new AtomicInteger(0);

    public int allocate() {
        return id.addAndGet(1);
    }
}
