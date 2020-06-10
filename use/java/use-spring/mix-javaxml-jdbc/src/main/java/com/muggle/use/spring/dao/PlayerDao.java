package com.muggle.use.spring.dao;

import com.muggle.use.spring.domain.Player;
import com.muggle.use.spring.domain.User;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.jdbc.core.RowMapper;
import org.springframework.jdbc.core.simple.SimpleJdbcInsert;
import org.springframework.stereotype.Repository;

import java.sql.Date;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

@Repository
public class PlayerDao {
    private JdbcTemplate jdbcTemplate;

    private static final String QUERY_PLAYER_BY_USER_ID =
            "select player_id, user_id, region, player_name, player_level from t_player where user_id=?";
    private static final String QUERY_PLAYER_BY_REGION_NAME =
            "select player_id, user_id, region, player_name, player_level from t_player where region=? and player_name=?";

    @Autowired
    public void setJdbcTemplate(JdbcTemplate jdbcTemplate) {
        this.jdbcTemplate = jdbcTemplate;
    }

    public List<Player> queryPlayerByUserId(int userId) {
        return jdbcTemplate.query(QUERY_PLAYER_BY_USER_ID, new PlayerRowMapper(), userId);
    }

    public List<Player> queryPlayerByRegionName(String region, String playerName) {
        return jdbcTemplate.query(QUERY_PLAYER_BY_REGION_NAME, new PlayerRowMapper(), region, playerName);
    }

    public int addPlayer(Player player) {
        SimpleJdbcInsert jdbcInsert = new SimpleJdbcInsert(jdbcTemplate).withTableName("t_player");
        jdbcInsert.setGeneratedKeyName("player_id");
        Map<String, Object> args = new HashMap<>();
        args.put("user_id", player.getUserId());
        args.put("region", player.getRegion());
        args.put("player_name", player.getPlayerName());
        args.put("player_level", player.getPlayerLevel());
        return jdbcInsert.executeAndReturnKey(args).intValue();
    }

    private static final class PlayerRowMapper implements RowMapper<Player> {
        @Override
        public Player mapRow(ResultSet rs, int rowNum) throws SQLException {
            int playerId = rs.getInt("player_id");
            int userId = rs.getInt("user_id");
            String region = rs.getString("region");
            String playerName = rs.getString("player_name");
            Integer playerLevelInteger = (Integer) rs.getObject("player_level");
            int playerLevel = 0;
            if (playerLevelInteger != null) {
                playerLevel = playerLevelInteger;
            }
            return new Player(playerId, userId, region, playerName, playerLevel);
        }
    }
}
