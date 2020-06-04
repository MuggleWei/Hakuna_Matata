package com.muggle.use.jdbc;

public class Player {
    private int playerId;
    private int userId;
    private String playerName;
    private int playerLevel;

    public Player() {
    }

    public Player(int playerId, int userId, String playerName, int playerLevel) {
        this.playerId = playerId;
        this.userId = userId;
        this.playerName = playerName;
        this.playerLevel = playerLevel;
    }

    public int getPlayerId() {
        return playerId;
    }

    public void setPlayerId(int playerId) {
        this.playerId = playerId;
    }

    public int getUserId() {
        return userId;
    }

    public void setUserId(int userId) {
        this.userId = userId;
    }

    public String getPlayerName() {
        return playerName;
    }

    public void setPlayerName(String playerName) {
        this.playerName = playerName;
    }

    public int getPlayerLevel() {
        return playerLevel;
    }

    public void setPlayerLevel(int playerLevel) {
        this.playerLevel = playerLevel;
    }

    @Override
    public String toString() {
        return "Player{" +
                "playerId=" + playerId +
                ", userId=" + userId +
                ", playerName='" + playerName + '\'' +
                ", playerLevel=" + playerLevel +
                '}';
    }
}
