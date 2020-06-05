package com.muggle.use.spring;

import org.springframework.beans.factory.annotation.Autowired;

public class Wizard implements Player {

    private Weapon weapon;

    @Autowired
    public void setWeapon(Weapon weapon) {
        this.weapon = weapon;
    }

    @Override
    public Weapon showWeapon() {
        System.out.println("hi, I'm a wizard!");
        weapon.attack();
        return weapon;
    }
}
