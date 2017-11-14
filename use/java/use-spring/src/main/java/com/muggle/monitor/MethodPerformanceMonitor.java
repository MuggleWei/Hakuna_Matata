package com.muggle.monitor;

public class MethodPerformanceMonitor {
    private String methodName;
    private long startTime;

    public MethodPerformanceMonitor(String methodName) {
        this.methodName = methodName;
        startTime = System.currentTimeMillis();
    }

    public String getMethodName() {
        return methodName;
    }

    public long getElapsedTime() {
        long endTime = System.currentTimeMillis();
        return endTime - startTime;
    }
}
