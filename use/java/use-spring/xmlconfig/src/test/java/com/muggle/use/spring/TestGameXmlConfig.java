package com.muggle.use.spring;

import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.extension.ExtendWith;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.test.context.ContextConfiguration;
import org.springframework.test.context.junit.jupiter.SpringExtension;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotNull;

@ExtendWith(SpringExtension.class)
@ContextConfiguration({"classpath:game.xml"})
public class TestGameXmlConfig {

    @Autowired
    private Weapon weapon;

    @Autowired
    private Player player;

    @Test
    public void weaponNotNull() {
        assertNotNull(weapon);
    }

    @Test
    public void playerNotNull() {
        assertNotNull(player);
    }

    @Test
    public void weaponIsSingleton() {
        assertEquals(player.showWeapon(), weapon);
    }
}
