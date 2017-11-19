package com.muggle.model;

import java.util.Date;

public class TradeRecord {
    private String id;
    private Long account_id;
    private String product_id;
    private Double price;
    private Long trade_time;
    private Date trade_date;

    public String getId() {
        return id;
    }

    public void setId(String id) {
        this.id = id;
    }

    public Long getAccount_id() {
        return account_id;
    }

    public void setAccount_id(Long account_id) {
        this.account_id = account_id;
    }

    public String getProduct_id() {
        return product_id;
    }

    public void setProduct_id(String product_id) {
        this.product_id = product_id;
    }

    public Double getPrice() {
        return price;
    }

    public void setPrice(Double price) {
        this.price = price;
    }

    public Long getTrade_time() {
        return trade_time;
    }

    public void setTrade_time(Long trade_time) {
        this.trade_time = trade_time;
    }

    public Date getTrade_date() {
        return trade_date;
    }

    public void setTrade_date(Date trade_date) {
        this.trade_date = trade_date;
    }

    public TradeRecord() {
    }
}
