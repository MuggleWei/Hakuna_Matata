package com.muggle.use.spring.service;

import com.muggle.use.spring.dao.PlayerDao;
import com.muggle.use.spring.dao.UserDao;
import com.muggle.use.spring.domain.Player;
import com.muggle.use.spring.domain.User;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.util.List;

@Service
public class AppService {
    private static Logger logger = LoggerFactory.getLogger(AppService.class);

    private UserDao userDao;
    private PlayerDao playerDao;

    @Autowired
    public void setUserDao(UserDao userDao) {
        this.userDao = userDao;
    }

    @Autowired
    public void setPlayerDao(PlayerDao playerDao) {
        this.playerDao = playerDao;
    }

    public List<User> queryUserByName(String userName) {
        return userDao.queryUserByName(userName);
    }

    public User queryUserById(int userId) {
        User user = null;
        List<User> users = userDao.queryUserById(userId);
        if (users.size() == 1) {
            user = users.get(0);
        } else if (users.size() > 1) {
            logger.error("duplicate user id " + userId);
        }
        return user;
    }

    public int addUser(User user) {
        int userId = -1;
        try {
            userId = userDao.addUser(user);
        } catch (Exception e) {
            logger.error(e.getMessage(), e);
        }
        return userId;
    }

    public List<Player> queryPlayerByUserId(int userId) {
        return playerDao.queryPlayerByUserId(userId);
    }

    public Player queryPlayerByRegionName(String region, String playerName) {
        Player player = null;
        List<Player> players = playerDao.queryPlayerByRegionName(region, playerName);
        if (players.size() == 1) {
            player = players.get(0);
        } else if (players.size() > 1) {
            logger.error("duplicate region player name " + region + ", " + playerName);
        }
        return player;
    }

    public int addPlayer(Player player) {
        int playerId = -1;
        try {
            playerId = playerDao.addPlayer(player);
        } catch (Exception e) {
            logger.error(e.getMessage(), e);
        }
        return playerId;
    }

    @Transactional
    public void createUserPlayer(User user, Player player) {
        int userId = userDao.addUser(user);
        user.setUserId(userId);

        player.setUserId(userId);
        int playerId = playerDao.addPlayer(player);
        player.setPlayerId(playerId);
    }
}
