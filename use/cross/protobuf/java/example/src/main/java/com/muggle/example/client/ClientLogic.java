package com.muggle.example.client;

public class ClientLogic {
    public static final int CONN_STATUS_UNCONNECT = 0;
    public static final int CONN_STATUS_CONNECTTED = 1;
    public static final int CONN_STATUS_LOGINED = 2;
    public static final int CONN_STATUS_LOGIN_FAILED = 3;

    private int connStatus = CONN_STATUS_UNCONNECT;

    public int getConnStatus() {
        return connStatus;
    }

    public void setConnStatus(int connStatus) {
        this.connStatus = connStatus;
    }
}
