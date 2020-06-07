package com.muggle.use.spring.aspect;

import org.aspectj.lang.ProceedingJoinPoint;
import org.aspectj.lang.annotation.Around;
import org.aspectj.lang.annotation.Aspect;
import org.aspectj.lang.annotation.Pointcut;
import org.springframework.core.annotation.Order;

@Aspect
@Order(0)
public class AroundAspect {

    @Pointcut("execution(** com.muggle.use.spring.task..*(..))" +
            "|| execution(** com.muggle.use.spring.invoke..*(..))")
    public void task() {
    }

    @Around("task()")
    public Object taskAround(ProceedingJoinPoint joinPoint) throws Throwable {
        String name = joinPoint.getTarget().getClass().getName() + "." + joinPoint.getSignature().getName();
        System.out.println("around " + name + " start");

        long startMillis = System.currentTimeMillis();
        Object result = joinPoint.proceed();
        long endMillis = System.currentTimeMillis();
        long elpasedMillis = endMillis - startMillis;

        System.out.println(name + " use ms: " + elpasedMillis);
        System.out.println("around " + name + " end");

        return result;
    }
}
