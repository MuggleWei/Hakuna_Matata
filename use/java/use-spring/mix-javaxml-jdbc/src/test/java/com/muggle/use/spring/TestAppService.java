package com.muggle.use.spring;

import com.muggle.use.spring.domain.Player;
import com.muggle.use.spring.domain.User;
import com.muggle.use.spring.service.AppService;
import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.extension.ExtendWith;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.test.context.ContextConfiguration;
import org.springframework.test.context.junit.jupiter.SpringExtension;

import java.sql.Date;
import java.util.List;

@ExtendWith(SpringExtension.class)
@ContextConfiguration("/test.xml") // jdbc:initialize-database will init db and tables
public class TestAppService {
    private static Logger logger = LoggerFactory.getLogger(TestAppService.class);

    @Autowired
    AppService appService;

    @Test
    public void testQueryUserByName() {
        List<User> users = appService.queryUserByName("notExists");
        Assertions.assertEquals(users.size(), 0);

        String userName = "testuser1";

        users = appService.queryUserByName(userName);
        Assertions.assertEquals(users.size(), 1);

        User user = users.get(0);
        Assertions.assertEquals(user.getUserName(), userName);
    }

    @Test
    public void testQueryUserById() {
        User user = appService.queryUserById(0);
        Assertions.assertNull(user);

        int userId = 1;

        user = appService.queryUserById(userId);
        Assertions.assertNotNull(user);
        Assertions.assertEquals(user.getUserId(), userId);
    }

    @Test
    public void testAddUser() {
        User user = new User(0, "devtestuser", "123456", new Date(System.currentTimeMillis()), "127.0.0.1");

        int userId = 0;

        userId = appService.addUser(user);
        Assertions.assertTrue(userId > 0);
    }

    public void testQueryPlayerByUserId() {
        int userId = 1;

        List<Player> players = appService.queryPlayerByUserId(userId);
        Assertions.assertTrue(players.size() > 0);

        players = appService.queryPlayerByUserId(0);
        Assertions.assertEquals(players.size(), 0);
    }

    @Test
    public void testQueryPlayerByRegionName() {
        String region = "Azeroth";
        String user = "testuser1";

        Player player = appService.queryPlayerByRegionName(region, user);
        Assertions.assertNotNull(player);

        player = appService.queryPlayerByRegionName(region, "notexists");
        Assertions.assertNull(player);
    }

    @Test
    public void testAddPlayer() {
        Player player = new Player(0, 1, "devtestregion", "devtestplayer", 0);

        int playerId = 0;

        playerId = appService.addPlayer(player);
        Assertions.assertTrue(playerId > 0);
    }

    @Test
    public void testAddUserPlayer() {
        String name = "muggle";
        String region = "Azeroth";

        User user = new User(0, name, "123456", new Date(System.currentTimeMillis()), "127.0.0.1");
        Player player = new Player(0, 1, region, name, 0);

        appService.createUserPlayer(user, player);

        // cause INDEX_region_player in sample.sql
        try {
            appService.createUserPlayer(user, player);
        } catch (Exception e) {
            logger.warn(e.getMessage());
        }

        List<User> users = appService.queryUserByName(name);
        Assertions.assertEquals(users.size(), 1); // test transactional in createUserPlayer
    }
}
