package com.muggle.dcuser.controller;

import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RestController;

@RestController
public class HealthCheckController {
    @RequestMapping(value = "/health", method = RequestMethod.GET)
    public int health() {
        return 200;
    }
}
