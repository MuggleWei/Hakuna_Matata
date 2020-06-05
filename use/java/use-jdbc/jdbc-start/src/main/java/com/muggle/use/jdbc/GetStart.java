package com.muggle.use.jdbc;

import java.sql.*;

public class GetStart {
    static private final String DRIVER_CLASS = "com.mysql.cj.jdbc.Driver";
    static private final String URL = "jdbc:mysql://127.0.0.1:23306/sample_db?useUnicode=true&characterEncoding=utf8";
    static private final String USER = "muggle";
    static private final String PASSWORD = "wsz123";

    public static void main(String[] args) {
        // 加载JDBC驱动
        try {
            Class.forName(DRIVER_CLASS);
        } catch (ClassNotFoundException e) {
            e.printStackTrace();
            return;
        }

        // 建立数据库连接
        try (Connection conn = DriverManager.getConnection(URL, USER, PASSWORD)) { // try-with-resources Connection
            String userName = "mugglewei";

            // 插入用户
            int userId = InsertUser(conn, userName);

            // 查询用户
            User user = QueryUser(conn, userName);
            assert userId == user.getUserId();

            // 删除用户
            DeleteUser(conn, user);
        } catch (SQLException throwables) {
            throwables.printStackTrace();
        }
    }

    public static int InsertUser(Connection conn, String userName) {
        User user = new User(0, userName, "123456", new Date(System.currentTimeMillis()), "127.0.0.1");
        Player player = new Player(0, 0, "Azeroth", userName + "_player1", 0);

        int retUserId = 0;
        try (Statement statement = conn.createStatement()) { // try-with-resources Statement
            // insert user
            String sql = String.format("insert into t_user (user_name, password, reg_date, reg_ip) values ('%s', %s, '%s', '%s')",
                    user.getUserName(), user.getPassword(), user.getRegDate(), user.getRegIp());
            int cnt = statement.executeUpdate(sql, Statement.RETURN_GENERATED_KEYS);
            ResultSet resultSet = statement.getGeneratedKeys();
            if (resultSet.next()) {
                int userId = resultSet.getInt(1);
                user.setUserId(userId);
                player.setUserId(userId);
                System.out.println("insert user count: " + cnt + ", user=" + user);
            }

            // insert player
            sql = String.format("insert into t_player (user_id, region, player_name, player_level) values (%d, '%s', '%s', %d)",
                    player.getUserId(), player.getRegion(), player.getPlayerName(), player.getPlayerLevel());
            cnt = statement.executeUpdate(sql, Statement.RETURN_GENERATED_KEYS);
            resultSet = statement.getGeneratedKeys();
            if (resultSet.next()) {
                int playerId = resultSet.getInt(1);
                player.setPlayerId(playerId);
                System.out.println("insert player count: " + cnt + ", player=" + player);
            }

            retUserId = user.getUserId();
        } catch (SQLException throwables) {
            throwables.printStackTrace();
        }

        return retUserId;
    }

    public static User QueryUser(Connection conn, String qryUserName) {
        User user = null;
        try (Statement statement = conn.createStatement()) { // try-with-resources Statement
            String sql = "select user_id, user_name, password, reg_date, reg_ip from t_user where user_name='" + qryUserName + "'";
            try (ResultSet resultSet = statement.executeQuery(sql)) { // try-with-resources ResultSet
                while (resultSet.next()) {
                    user = new User();
                    Integer userId = (Integer) resultSet.getObject("user_id");
                    String userName = resultSet.getString("user_name");
                    String password = resultSet.getString("password");
                    Date regDate = resultSet.getDate("reg_date");
                    String regIp = resultSet.getString("reg_ip");

                    user.setUserId(userId);
                    user.setUserName(userName);
                    user.setPassword(password);
                    user.setRegDate(regDate);
                    user.setRegIp(regIp);

                    System.out.println("query user: " + user);
                }
            }
        } catch (SQLException throwables) {
            throwables.printStackTrace();
        }

        return user;
    }

    public static void DeleteUser(Connection conn, User user) {
        try (Statement statement = conn.createStatement()) { // try-with-resources Statement
            String sql = "delete from t_user where user_id = " + user.getUserId();
            int cnt = statement.executeUpdate(sql);
            System.out.println("delete user count: " + cnt);

            sql = "delete from t_player where user_id = " + user.getUserId();
            cnt = statement.executeUpdate(sql);
            System.out.println("delete player count: " + cnt);
        } catch (SQLException throwables) {
            throwables.printStackTrace();
        }
    }
}
