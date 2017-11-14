package com.muggle.beanconfig;

import com.muggle.object.Car;
import com.muggle.object.Driver;
import org.springframework.beans.factory.annotation.Qualifier;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.context.annotation.Scope;

@Configuration
public class ObjectBeans {

    @Bean("hongqi")
    public Car getCarHongqi() {
        Car car = new Car();
        car.setBrand("hongqi");
        car.setColor("black");
        car.setMaxSpeed(300);
        return car;
    }

    @Bean("Audi")
    @Scope(value = "prototype")
    public Car getCarAudi() {
        Car car = new Car();
        car.setBrand("Audi");
        car.setColor("white");
        car.setMaxSpeed(280);
        return car;
    }

    @Bean("driver")
    public Driver getDriver(@Qualifier(value = "hongqi") Car car) {
        Driver driver = new Driver();
        driver.setName("unknown");
        driver.setCar(car);
        return driver;
    }

}
