package com.muggle.aop;

import com.muggle.monitor.MethodPerformanceMonitor;
import org.aspectj.lang.JoinPoint;
import org.aspectj.lang.annotation.After;
import org.aspectj.lang.annotation.Aspect;
import org.aspectj.lang.annotation.Before;

@Aspect
public class PerformanceMonitor {
    private ThreadLocal<MethodPerformanceMonitor> methodMonitor = new ThreadLocal<MethodPerformanceMonitor>();

    public PerformanceMonitor() {
    }

    @Before("execution (* com.muggle.object..*(..))")
    public void begin(JoinPoint joinPoint) {
        String name = joinPoint.getTarget().getClass().getName() + "." + joinPoint.getSignature().getName();
        methodMonitor.set(new MethodPerformanceMonitor(name));
    }

    @After("execution (* com.muggle.object..*(..))")
    public void end() {
        long elapseTime = methodMonitor.get().getElapsedTime();
        String methodName = methodMonitor.get().getMethodName();
        System.out.println(methodName + ": " + elapseTime + "ms");
    }
}
