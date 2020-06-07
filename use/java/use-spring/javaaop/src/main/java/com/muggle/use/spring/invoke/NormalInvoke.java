package com.muggle.use.spring.invoke;

import com.muggle.use.spring.InvokeArgs;
import org.springframework.stereotype.Component;

@Component
public class NormalInvoke implements Invoke {
    @Override
    public void call(InvokeArgs index) {
        System.out.println("normal invoke call(" + index.getIndex() + ")");
    }
}
