package com.muggle.hello;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.cloud.client.ServiceInstance;
import org.springframework.cloud.client.discovery.DiscoveryClient;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RestController;

@RestController
public class HelloController {

    @Autowired
    private DiscoveryClient discoveryClient;

    @RequestMapping(value = "/health", method = RequestMethod.GET)
    public int health() {
        return 200;
    }

    @RequestMapping(value = "/greet/{your-name}")
    public String hello(@PathVariable("your-name") String name) {
        ServiceInstance serviceInstance = discoveryClient.getLocalServiceInstance();
        String info = serviceInstance.getServiceId() + "<" +
                serviceInstance.getHost() + ":" + serviceInstance.getPort() + ">";
        return "Hello " + name + " [from " + info + "]";
    }

}
