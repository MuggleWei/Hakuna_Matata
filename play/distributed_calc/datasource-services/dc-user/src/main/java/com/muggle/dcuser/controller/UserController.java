package com.muggle.dcuser.controller;

import com.muggle.common.utils.ErrorIdUtils;
import com.muggle.common.utils.ReturnMsgUtils;
import com.muggle.dcuser.componenet.UserStatusUtils;
import com.muggle.dcuser.entity.Authority;
import com.muggle.dcuser.entity.User;
import com.muggle.dcuser.model.UserModel;
import com.muggle.dcuser.model.UserPasswdModel;
import com.muggle.dcuser.model.UserUpdateModel;
import com.muggle.dcuser.service.UserService;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;

@RestController
@RequestMapping("/v1/user")
public class UserController {
    private final Logger logger = LoggerFactory.getLogger(this.getClass());

    @Autowired
    private UserService userService;

    @Autowired
    private ReturnMsgUtils returnMsgUtils;

    @Autowired
    private ErrorIdUtils errorIdUtils;

    @Autowired
    private UserStatusUtils userStatusUtils;

    @RequestMapping(value = "/query/info", method = RequestMethod.GET)
    public Object queryUser(
            @RequestParam(value = "id", required = false) Long userId,
            @RequestParam(value = "name", required = false) String userName) {
        UserModel userModel = new UserModel();

        User user = new User();
        user.setId(userId);
        user.setName(userName);
        user = userService.queryUser(user);
        if (user == null) {
            userModel.setId(userId);
            userModel.setName(userName);
            return returnMsgUtils.failedMap(userModel, errorIdUtils.errIdNotFound);
        }

        Authority authority = userService.queryUserAuthority(user.getId());
        if (authority == null) {
            userModel.setId(user.getId());
            userModel.setName(user.getName());
            return returnMsgUtils.failedMap(
                    userModel, errorIdUtils.errIdNotFound, "authority not found");
        }

        userModel.setId(user.getId());
        userModel.setName(user.getName());
        userModel.setStatus(userStatusUtils.strStatus(user.getStatus()));
        userModel.setNumTasks(authority.getNumTasks());
        return returnMsgUtils.success(userModel);
    }

    @RequestMapping(value = "/add", method = RequestMethod.POST)
    public Object addUser(
            @RequestBody UserPasswdModel userPasswdModel) {
        int ret = 0;

        User user = new User();
        user.setName(userPasswdModel.getName());
        user.setPassword(userPasswdModel.getPassword());
        if (userService.queryUser(user) != null) {
            return returnMsgUtils.failedMap(userPasswdModel, errorIdUtils.errIdDuplicateName);
        }

        try {
            ret = userService.addUser(user);
            userPasswdModel.setId(user.getId());
        } catch (Exception e) {
            ret = -1;
        }

        userPasswdModel.setPassword(null);
        if (ret == errorIdUtils.ok) {
            return returnMsgUtils.success(userPasswdModel);
        } else {
            return returnMsgUtils.failedMap(userPasswdModel, ret);
        }
    }

    @RequestMapping(value = "/freeze", method = RequestMethod.POST)
    public Object freezeUser(
            @RequestBody UserModel userModel) {

        User targetUser = new User();
        targetUser.setId(userModel.getId());
        targetUser.setName(userModel.getName());

        User updateUser = new User();
        updateUser.setStatus(userStatusUtils.freeze);

        int ret = userService.updateUser(targetUser, updateUser);
        if (ret == errorIdUtils.ok) {
            return returnMsgUtils.success(userModel);
        } else {
            return returnMsgUtils.failedMap(userModel, ret);
        }
    }

    @RequestMapping(value = "/update/password", method = RequestMethod.POST)
    public Object updatePassword(
            @RequestBody UserPasswdModel userPasswdModel) {
        User user = new User();
        user.setId(userPasswdModel.getId());
        user.setName(userPasswdModel.getName());
        user.setPassword(userPasswdModel.getPassword());

        int ret = userService.checkUserPassword(user);
        if (ret != errorIdUtils.ok) {
            userPasswdModel.setPassword(null);
            userPasswdModel.setNewPassword(null);
            return returnMsgUtils.failedMap(userPasswdModel, ret);
        }

        ret = userService.updatePassword(user, userPasswdModel.getNewPassword());
        userPasswdModel.setPassword(null);
        userPasswdModel.setNewPassword(null);
        if (ret == errorIdUtils.ok) {
            return returnMsgUtils.success(userPasswdModel);
        } else {
            return returnMsgUtils.failedMap(userPasswdModel, ret);
        }
    }

    @RequestMapping(value = "/update/info", method = RequestMethod.POST)
    public Object updateUserInfo(
            @RequestBody UserUpdateModel userUpdateModel) {
        User user = new User();
        user.setId(userUpdateModel.getId());
        user.setName(userUpdateModel.getName());
        if (user.getId() == null && user.getName() == null) {
            return returnMsgUtils.failedMap(userUpdateModel, errorIdUtils.errIdInvalidParam);
        }

        User rspUser = userService.queryUser(user);
        if (rspUser == null) {
            return returnMsgUtils.failedMap(userUpdateModel, errorIdUtils.errIdNotFound);
        }
        user.setId(rspUser.getId());
        user.setName(rspUser.getName());

        User userInfo = new User();
        userInfo.setName(userUpdateModel.getNewName());
        userInfo.setStatus(userStatusUtils.intStatus(userUpdateModel.getNewStatus()));
        int ret = -1;
        try {
            ret = userService.updateUser(user, userInfo);
        } catch (Exception e) {
            ret = errorIdUtils.errIdDuplicateName;
        }

        if (userUpdateModel.getNewNumTasks() != null) {
            Authority authority = new Authority();
            authority.setUserId(user.getId());
            authority.setNumTasks(userUpdateModel.getNewNumTasks());
            userService.updateAuthority(authority);
        }

        if (ret == errorIdUtils.ok) {
            return returnMsgUtils.success(userUpdateModel);
        } else {
            return returnMsgUtils.failedMap(userUpdateModel, ret);
        }
    }
}
