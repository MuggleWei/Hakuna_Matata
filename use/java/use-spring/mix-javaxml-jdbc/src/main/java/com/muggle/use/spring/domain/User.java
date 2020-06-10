package com.muggle.use.spring.domain;

import java.sql.Date;

public class User {
    private int userId;
    private String userName;
    private String password;
    private Date regDate;
    private String regIp;

    public User() {
    }

    public User(int userId, String userName, String password, Date regDate, String regIp) {
        this.userId = userId;
        this.userName = userName;
        this.password = password;
        this.regDate = regDate;
        this.regIp = regIp;
    }

    public int getUserId() {
        return userId;
    }

    public void setUserId(int userId) {
        this.userId = userId;
    }

    public String getUserName() {
        return userName;
    }

    public void setUserName(String userName) {
        this.userName = userName;
    }

    public String getPassword() {
        return password;
    }

    public void setPassword(String password) {
        this.password = password;
    }

    public Date getRegDate() {
        return regDate;
    }

    public void setRegDate(Date regDate) {
        this.regDate = regDate;
    }

    public String getRegIp() {
        return regIp;
    }

    public void setRegIp(String regIp) {
        this.regIp = regIp;
    }
}
