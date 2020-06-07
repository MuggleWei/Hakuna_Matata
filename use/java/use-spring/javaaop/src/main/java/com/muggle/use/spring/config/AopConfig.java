package com.muggle.use.spring.config;

import com.muggle.use.spring.aspect.TaskBeforeAfterAspect;
import com.muggle.use.spring.invoke.Invoke;
import com.muggle.use.spring.task.Task;
import org.springframework.context.annotation.ComponentScan;
import org.springframework.context.annotation.Configuration;
import org.springframework.context.annotation.EnableAspectJAutoProxy;

@Configuration
@EnableAspectJAutoProxy
@ComponentScan(basePackageClasses = {Task.class, Invoke.class, TaskBeforeAfterAspect.class})
public class AopConfig {
}
