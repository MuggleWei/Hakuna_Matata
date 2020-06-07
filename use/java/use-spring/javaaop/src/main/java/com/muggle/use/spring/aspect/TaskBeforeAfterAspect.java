package com.muggle.use.spring.aspect;

import org.aspectj.lang.JoinPoint;
import org.aspectj.lang.annotation.*;
import org.springframework.core.annotation.Order;

@Aspect
@Order(1)
public class TaskBeforeAfterAspect {
    @Pointcut("execution(** com.muggle.use.spring.task..*(..))")
    public void task() {
    }

    @Before("task()")
    public void before(JoinPoint joinPoint) {
        String name = joinPoint.getTarget().getClass().getName() + "." + joinPoint.getSignature().getName();
        System.out.println("before " + name);
    }

    @AfterReturning("task()")
    public void afterReturning(JoinPoint joinPoint) {
        String name = joinPoint.getTarget().getClass().getName() + "." + joinPoint.getSignature().getName();
        System.out.println("after return " + name);
    }

    @AfterThrowing("task()")
    public void afterThrowing(JoinPoint joinPoint) {
        String name = joinPoint.getTarget().getClass().getName() + "." + joinPoint.getSignature().getName();
        System.out.println("after throw " + name);
    }
}
