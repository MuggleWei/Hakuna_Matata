package com.muggle.object;

public class Car {
    private String brand;
    private String color;
    private int maxSpeed;

    public String getBrand() {
        return brand;
    }

    public void setBrand(String brand) {
        this.brand = brand;
    }

    public String getColor() {
        return color;
    }

    public void setColor(String color) {
        this.color = color;
    }

    public int getMaxSpeed() {
        return maxSpeed;
    }

    public void setMaxSpeed(int maxSpeed) {
        this.maxSpeed = maxSpeed;
    }

    public Car() {
    }

    public void Introduce() {
        System.out.println("car: " + brand + ", " + color + ", " + maxSpeed);
    }
}
