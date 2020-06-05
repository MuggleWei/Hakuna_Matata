package com.muggle.use.spring.config;

import org.springframework.context.annotation.ComponentScan;
import org.springframework.context.annotation.Configuration;

@Configuration
@ComponentScan(basePackageClasses = {com.muggle.use.spring.Player.class})
public class GameConfig {
}
