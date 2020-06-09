package com.muggle.use.spring.domain;

import javax.validation.constraints.NotNull;
import javax.validation.constraints.Size;

public class RegisterInfo {
    @NotNull
    @Size(min = 3, max = 32)
    private String userName;

    @NotNull
    @Size(min = 6, max = 28)
    private String passWord;

    private String phoneNumber;
    private String email;

    public RegisterInfo() {
    }

    public RegisterInfo(@NotNull @Size(min = 3, max = 32) String userName, @NotNull @Size(min = 6, max = 28) String passWord, String phoneNumber, String email) {
        this.userName = userName;
        this.passWord = passWord;
        this.phoneNumber = phoneNumber;
        this.email = email;
    }

    public String getUserName() {
        return userName;
    }

    public void setUserName(String userName) {
        this.userName = userName;
    }

    public String getPassWord() {
        return passWord;
    }

    public void setPassWord(String passWord) {
        this.passWord = passWord;
    }

    public String getPhoneNumber() {
        return phoneNumber;
    }

    public void setPhoneNumber(String phoneNumber) {
        this.phoneNumber = phoneNumber;
    }

    public String getEmail() {
        return email;
    }

    public void setEmail(String email) {
        this.email = email;
    }
}
