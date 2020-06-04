package com.muggle.use.jdbc;

public class Asset {
    private int playerId;
    private int assetType;
    private int assetVal;

    public Asset() {
    }

    public Asset(int playerId, int assetType, int assetVal) {
        this.playerId = playerId;
        this.assetType = assetType;
        this.assetVal = assetVal;
    }

    public int getPlayerId() {
        return playerId;
    }

    public void setPlayerId(int playerId) {
        this.playerId = playerId;
    }

    public int getAssetType() {
        return assetType;
    }

    public void setAssetType(int assetType) {
        this.assetType = assetType;
    }

    public int getAssetVal() {
        return assetVal;
    }

    public void setAssetVal(int assetVal) {
        this.assetVal = assetVal;
    }

    @Override
    public String toString() {
        return "Asset{" +
                "playerId=" + playerId +
                ", assetType=" + assetType +
                ", assetVal=" + assetVal +
                '}';
    }
}
