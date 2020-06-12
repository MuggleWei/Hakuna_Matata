package com.muggle.use.mybatis.model;

import java.sql.Timestamp;

public class User {
    private Long userId;
    private String userName;
    private Timestamp regDate;
    private String regIp;

    public User() {
    }

    public User(Long userId, String userName, Timestamp regDate, String regIp) {
        this.userId = userId;
        this.userName = userName;
        this.regDate = regDate;
        this.regIp = regIp;
    }

    public Long getUserId() {
        return userId;
    }

    public void setUserId(Long userId) {
        this.userId = userId;
    }

    public String getUserName() {
        return userName;
    }

    public void setUserName(String userName) {
        this.userName = userName;
    }

    public Timestamp getRegDate() {
        return regDate;
    }

    public void setRegDate(Timestamp regDate) {
        this.regDate = regDate;
    }

    public String getRegIp() {
        return regIp;
    }

    public void setRegIp(String regIp) {
        this.regIp = regIp;
    }
}
