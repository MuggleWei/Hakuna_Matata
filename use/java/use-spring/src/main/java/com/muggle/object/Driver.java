package com.muggle.object;

public class Driver {
    private String name;
    private Car car;

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public Car getCar() {
        return car;
    }

    public void setCar(Car car) {
        this.car = car;
    }

    public Driver() {
    }

    public void Introduce() {
        System.out.print("driver " + name + " has ");
        car.Introduce();
    }
}
