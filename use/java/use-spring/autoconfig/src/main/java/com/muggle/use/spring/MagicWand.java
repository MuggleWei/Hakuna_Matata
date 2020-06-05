package com.muggle.use.spring;

import org.springframework.stereotype.Component;

@Component
public class MagicWand implements Weapon {

    @Override
    public void attack() {
        System.out.println("magic wand attack");
    }
}
