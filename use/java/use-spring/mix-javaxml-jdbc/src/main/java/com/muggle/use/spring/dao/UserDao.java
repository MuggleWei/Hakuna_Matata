package com.muggle.use.spring.dao;

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
public class UserDao {
    private JdbcTemplate jdbcTemplate;

    private static final String QUERY_USER_BY_NAME =
            "select user_id, user_name, password, reg_date, reg_ip from t_user where user_name=?";
    private static final String QUERY_USER_BY_ID =
            "select user_id, user_name, password, reg_date, reg_ip from t_user where user_id=?";

    @Autowired
    public void setJdbcTemplate(JdbcTemplate jdbcTemplate) {
        this.jdbcTemplate = jdbcTemplate;
    }

    public List<User> queryUserByName(String userName) {
        return jdbcTemplate.query(QUERY_USER_BY_NAME, new UserRowMapper(), userName);
    }

    public List<User> queryUserById(int userId) throws RuntimeException {
        return jdbcTemplate.query(QUERY_USER_BY_ID, new UserRowMapper(), userId);
    }

    public int addUser(User user) {
        SimpleJdbcInsert jdbcInsert = new SimpleJdbcInsert(jdbcTemplate).withTableName("t_user");
        jdbcInsert.setGeneratedKeyName("user_id");
        Map<String, Object> args = new HashMap<>();
        args.put("user_name", user.getUserName());
        args.put("password", user.getPassword());
        args.put("reg_date", user.getRegDate());
        args.put("reg_ip", user.getRegIp());
        return jdbcInsert.executeAndReturnKey(args).intValue();
    }

    private static final class UserRowMapper implements RowMapper<User> {
        @Override
        public User mapRow(ResultSet rs, int rowNum) throws SQLException {
            int userId = rs.getInt("user_id");
            String userName = rs.getString("user_name");
            String password = rs.getString("password");
            Date regDate = rs.getDate("reg_date");
            String regIp = rs.getString("reg_ip");
            return new User(userId, userName, password, regDate, regIp);
        }
    }
}
