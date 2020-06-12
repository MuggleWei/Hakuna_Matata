package com.muggle.use.mybatis.model;

public class Fund {
    private Long userId;
    private Integer currencyType;
    private Long amount;

    public Fund() {
    }

    public Fund(Long userId, Integer currencyType, Long amount) {
        this.userId = userId;
        this.currencyType = currencyType;
        this.amount = amount;
    }

    public Long getUserId() {
        return userId;
    }

    public void setUserId(Long userId) {
        this.userId = userId;
    }

    public Integer getCurrencyType() {
        return currencyType;
    }

    public void setCurrencyType(Integer currencyType) {
        this.currencyType = currencyType;
    }

    public Long getAmount() {
        return amount;
    }

    public void setAmount(Long amount) {
        this.amount = amount;
    }
}
