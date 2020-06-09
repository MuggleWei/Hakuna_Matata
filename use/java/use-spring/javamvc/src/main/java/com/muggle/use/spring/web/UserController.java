package com.muggle.use.spring.web;

import com.muggle.use.spring.domain.User;
import com.muggle.use.spring.service.UserService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RequestParam;

import java.util.List;

@Controller
@RequestMapping("/user")
public class UserController {
    private UserService userService;

    @Autowired
    public void setUserService(UserService userService) {
        this.userService = userService;
    }

//    @RequestMapping(method = RequestMethod.GET)
//    public String users(Model model) {
//        List<User> users = userService.getUsers(1, 20);
//        model.addAttribute("userList", users);
//        return "users";
//    }

//    @RequestMapping(method = RequestMethod.GET)
//    public List<User> users() {
//        return userService.getUsers(1, 20);
//    }

    @RequestMapping(value = "/all", method = RequestMethod.GET)
    public String users(
            @RequestParam(value = "page", defaultValue = "1") int page,
            @RequestParam(value = "user_per_page", defaultValue = "20") int userPerPage,
            Model model) {
        List<User> users = userService.getUsers(page, userPerPage);
        model.addAttribute("userList", users);
        return "users";
    }

    @RequestMapping(method = RequestMethod.GET)
    public User user(
            @RequestParam(value = "user_name") String userName,
            Model model) {
        return userService.findUserByName(userName);
    }

    @RequestMapping(value = "/{userId}", method = RequestMethod.GET)
    public String userId(
            @PathVariable(value = "userId") int userId,
            Model model) {
        User user = userService.findUserById(userId);
        model.addAttribute("user", user);
        return "user";
    }
}
