package com.muggle.use.spring.aspect;

import com.muggle.use.spring.InvokeArgs;
import org.aspectj.lang.annotation.Aspect;
import org.aspectj.lang.annotation.Before;
import org.aspectj.lang.annotation.Pointcut;

@Aspect
public class InvokeBeforeAfterAspect {

    @Pointcut("execution(** com.muggle.use.spring..call(com.muggle.use.spring.InvokeArgs)) && args(idx)")
    public void invoke(InvokeArgs idx) {
    }

    @Before("invoke(idx)")
    public void beforeInvoke(InvokeArgs idx) {
        System.out.println("beforeInvoke (" + idx.getIndex() + ")");
        idx.setIndex(idx.getIndex() % 8);
    }

}
