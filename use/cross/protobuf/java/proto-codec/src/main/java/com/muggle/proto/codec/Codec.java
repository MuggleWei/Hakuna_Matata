package com.muggle.proto.codec;

public class Codec {
    public static final String version = "0.0.1";

    private int nameLenLimit = 128;
    private int totalLenLimit = 4096;

    public int getNameLenLimit() {
        return nameLenLimit;
    }

    public void setNameLenLimit(int nameLenLimit) {
        this.nameLenLimit = nameLenLimit;
    }

    public int getTotalLenLimit() {
        return totalLenLimit;
    }

    public void setTotalLenLimit(int totalLenLimit) {
        this.totalLenLimit = totalLenLimit;
    }
}
