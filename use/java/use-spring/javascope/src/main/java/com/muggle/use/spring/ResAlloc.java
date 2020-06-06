package com.muggle.use.spring;

import org.springframework.stereotype.Component;

import java.util.concurrent.LinkedBlockingQueue;

@Component
public class ResAlloc {
    private static int COUNT_CPU = 4;
    private static int COUNT_MEM = 8;

    private LinkedBlockingQueue<Integer> cpuQueue = new LinkedBlockingQueue<>();
    private LinkedBlockingQueue<Integer> memQueue = new LinkedBlockingQueue<>();

    public ResAlloc() {
        for (int i = 0; i < COUNT_CPU; i++) {
            cpuQueue.add(i);
        }

        for (int i = 0; i < COUNT_MEM; i++) {
            memQueue.add(i);
        }
    }

    public Integer allcCpu() throws InterruptedException {
        return cpuQueue.take();
    }

    public void freeCpu(Integer res) {
        cpuQueue.offer(res);
    }

    public Integer allcMem() throws InterruptedException {
        return memQueue.take();
    }

    public void freeMem(Integer res) {
        memQueue.offer(res);
    }

}
