package com.muggle.use.spring;

import com.muggle.use.spring.config.AopConfig;
import com.muggle.use.spring.invoke.NormalInvoke;
import com.muggle.use.spring.task.BadTask;
import com.muggle.use.spring.task.NormalTask;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.extension.ExtendWith;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.test.context.ContextConfiguration;
import org.springframework.test.context.junit.jupiter.SpringExtension;

@ExtendWith(SpringExtension.class)
@ContextConfiguration(classes = {AopConfig.class})
public class TestAop {
    @Autowired
    private NormalTask normalTask;

    @Autowired
    private BadTask badTask;

    @Autowired
    private NormalInvoke normalInvoke;

    @Test
    public void testTaskAspect() {
        normalTask.run();
        try {
            badTask.run();
        } catch (RuntimeException e) {
            System.out.println(e.getMessage());
        }
    }

    @Test
    public void testInvokeAspect() {
        InvokeArgs args = new InvokeArgs();
        args.setIndex(16);
        normalInvoke.call(args);
    }
}
