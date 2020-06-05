package com.muggle.use.spring;

import org.springframework.beans.factory.annotation.Autowired;

public class Knight implements Player {
    private Weapon weapon;

    @Autowired
    public void setWeapon(Weapon weapon) {
        this.weapon = weapon;
    }

    @Override
    public Weapon showWeapon() {
        System.out.println("hi, I'm a knight!");
        weapon.attack();
        return weapon;
    }
}
