package com.muggle.dcuser.componenet;

import org.springframework.stereotype.Component;

import java.util.Objects;

@Component
public class UserStatusUtils {
    public final int normal = 0;
    public final int freeze = 1;

    public String strStatus(Integer status) {
        if (status == null) {
            return "unknown";
        }

        if (status == normal) {
            return "normal";
        } else if (status == freeze) {
            return "freeze";
        } else {
            return "unknown";
        }
    }

    public Integer intStatus(String status) {
        if (Objects.equals(status, "normal")) {
            return normal;
        } else if (Objects.equals(status, "freeze")) {
            return freeze;
        } else {
            return null;
        }
    }
}
