package com.muggle.use.spring.config;

import com.muggle.use.spring.Knight;
import com.muggle.use.spring.Player;
import com.muggle.use.spring.Sword;
import com.muggle.use.spring.Weapon;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

@Configuration
public class GameConfig {
    @Bean
    public Weapon weapon() {
        return new Sword();
    }

    @Bean
    public Player player() {
        return new Knight();
    }
}
