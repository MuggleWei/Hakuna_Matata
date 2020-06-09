package com.muggle.use.spring.web;

import com.muggle.use.spring.domain.RegisterInfo;
import com.muggle.use.spring.service.UserService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.validation.Errors;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;

import javax.validation.Valid;

@Controller
public class RegisterController {
    private UserService userService;

    @Autowired
    public void setUserService(UserService userService) {
        this.userService = userService;
    }

    @RequestMapping(value = "register", method = RequestMethod.GET)
    public String showRegistrationForm() {
        return "registerForm";
    }

    @RequestMapping(value = "register", method = RequestMethod.POST)
    public String processRegistration(
            @Valid RegisterInfo registerInfo,
            Errors errors) {
        if (errors.hasErrors()) {
            System.out.println(errors);
            return "register";
        }

        int userId = userService.addUser(registerInfo);
        return "redirect:/user/" + userId;
    }
}
