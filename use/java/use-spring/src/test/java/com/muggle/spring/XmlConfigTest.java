package com.muggle.spring;

import com.muggle.object.Car;
import com.muggle.object.Driver;
import org.junit.Assert;
import org.junit.Test;
import org.springframework.context.ApplicationContext;
import org.springframework.context.support.ClassPathXmlApplicationContext;

public class XmlConfigTest {
    @Test
    public void XmlBeanTest() {
        ApplicationContext ctx = new ClassPathXmlApplicationContext(new String[]{
                "objectbeans.xml"
        });

        Car hongqiCar = ctx.getBean("hongqi", Car.class);
        Assert.assertNotNull(hongqiCar);
        Assert.assertEquals(hongqiCar.getBrand(), "hongqi");

        Car audiCar = ctx.getBean("Audi", Car.class);
        Assert.assertNotNull(audiCar);
        Assert.assertEquals(audiCar.getBrand(), "Audi");

        Driver driver = ctx.getBean("driver", Driver.class);
        Assert.assertNotNull(driver);
        Assert.assertEquals(driver.getCar(), hongqiCar);

        Car hongqiCar2 = ctx.getBean("hongqi", Car.class);
        Assert.assertNotNull(hongqiCar2);
        Assert.assertEquals(hongqiCar2.getBrand(), "hongqi");
        Assert.assertEquals(hongqiCar, hongqiCar2);

        Car audiCar2 = ctx.getBean("Audi", Car.class);
        Assert.assertNotNull(audiCar2);
        Assert.assertEquals(audiCar2.getBrand(), "Audi");
        Assert.assertNotEquals(audiCar, audiCar2);
    }

    @Test
    public void XmlAspectTest() {
        ApplicationContext ctx = new ClassPathXmlApplicationContext(new String[]{
                "objectbeans.xml", "aspectbeans.xml"
        });

        Car hongqiCar = ctx.getBean("hongqi", Car.class);
        Assert.assertNotNull(hongqiCar);
        Assert.assertEquals(hongqiCar.getBrand(), "hongqi");

        Car audiCar = ctx.getBean("Audi", Car.class);
        Assert.assertNotNull(audiCar);
        Assert.assertEquals(audiCar.getBrand(), "Audi");

        Driver driver = ctx.getBean("driver", Driver.class);
        Assert.assertNotNull(driver);
        Assert.assertEquals(driver.getCar(), hongqiCar);

        hongqiCar.Introduce();
        audiCar.Introduce();
        driver.Introduce();
    }
}
