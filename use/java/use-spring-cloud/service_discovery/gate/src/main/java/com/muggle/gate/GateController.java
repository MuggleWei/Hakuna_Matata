package com.muggle.gate;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.cloud.client.ServiceInstance;
import org.springframework.cloud.client.discovery.DiscoveryClient;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RestController;
import org.springframework.web.client.RestTemplate;

import java.util.ArrayList;
import java.util.List;

@RestController
public class GateController {

    @Autowired
    private DiscoveryClient discoveryClient;

    @Autowired
    RestTemplate restTemplate;

    @RequestMapping(value = "/health", method = RequestMethod.GET)
    public int health() {
        return 200;
    }

    @RequestMapping(value = "/", method = RequestMethod.GET)
    public String index() {
        return "Welcome, this is a tiny example for service register and discovery. " +
                "You can enter /service/{service-name}, to find all {service-name} service nodes. " +
                "You can enter /hello/{your name}, gate will discovery hello services from consul, " +
                "and send request to one of them. (NOTE: this service is another example of this project)";
    }

    @RequestMapping(value = "/service/{service-name}")
    public List<String> services(@PathVariable("service-name") String name) {
        List<ServiceInstance> serviceInstanceList = discoveryClient.getInstances(name);
        List<String> intros = new ArrayList<>();
        if (serviceInstanceList != null && serviceInstanceList.size() > 0) {
            for (ServiceInstance serviceInstance : serviceInstanceList) {
                String metaString = "";
                for (String val : serviceInstance.getMetadata().values()) {
                    metaString = metaString + val + " ";
                }
                intros.add(serviceInstance.getServiceId() + " | "
                        + serviceInstance.getHost() + ":" + serviceInstance.getPort() + " | "
                        + metaString);
            }
        }

        return intros;
    }

    @RequestMapping(value = "/hello/{your-name}")
    public String hello(@PathVariable("your-name") String name) {
        if (name.length() == 0) {
            return "";
        }
        String url = "http://hello/greet/" + name;
        return restTemplate.getForEntity(url, String.class).getBody();
    }
}
