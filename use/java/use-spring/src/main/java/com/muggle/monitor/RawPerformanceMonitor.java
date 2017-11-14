package com.muggle.monitor;

import org.aspectj.lang.JoinPoint;

public class RawPerformanceMonitor {
    private ThreadLocal<MethodPerformanceMonitor> methodMonitor = new ThreadLocal<MethodPerformanceMonitor>();

    public RawPerformanceMonitor() {
    }

    public void begin(JoinPoint joinPoint) {
        String name = joinPoint.getTarget().getClass().getName() + "." + joinPoint.getSignature().getName();
        methodMonitor.set(new MethodPerformanceMonitor(name));
    }

    public void end() {
        long elapseTime = methodMonitor.get().getElapsedTime();
        String methodName = methodMonitor.get().getMethodName();
        System.out.println(methodName + ": " + elapseTime + "ms");
    }
}
