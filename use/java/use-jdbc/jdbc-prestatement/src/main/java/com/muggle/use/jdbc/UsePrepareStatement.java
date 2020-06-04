package com.muggle.use.jdbc;

import java.sql.*;

public class UsePrepareStatement {
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
            if (userId == 0) {
                System.out.println("failed insert user");
                System.exit(1);
            }

            // 查询用户
            User user = QueryUser(conn, userName);
            if (user == null) {
                System.out.println("failed query user");
                if (userId != user.getUserId()) {
                    System.out.println("incorrect user id");
                }
            }

            // 删除用户
            DeleteUser(conn, user);
        } catch (SQLException throwables) {
            throwables.printStackTrace();
        }
    }

    public static int InsertUser(Connection conn, String userName) {
        User user = new User(0, userName, "123456", new Date(System.currentTimeMillis()), "127.0.0.1");
        Player player = new Player(0, 0, userName + "_player1", 0);

        String sqlInsertUser = "insert into t_user (user_name, password, reg_date, reg_ip) values (?, ?, ?, ?)";
        String sqlInsertPlayer = "insert into t_player (user_id, player_name, player_level) values (?, ?, ?)";

        try (PreparedStatement userPreparedStatement = conn.prepareStatement(sqlInsertUser, Statement.RETURN_GENERATED_KEYS);
             PreparedStatement playerPreparedStatement = conn.prepareStatement(sqlInsertPlayer, Statement.RETURN_GENERATED_KEYS)) { // try-with-resources Statement
            // insert user
            userPreparedStatement.setString(1, user.getUserName());
            userPreparedStatement.setString(2, user.getPassword());
            userPreparedStatement.setDate(3, user.getRegDate());
            userPreparedStatement.setString(4, user.getRegIp());
            int cnt = userPreparedStatement.executeUpdate();
            ResultSet resultSet = userPreparedStatement.getGeneratedKeys();
            if (resultSet.next()) {
                int userId = resultSet.getInt(1);
                user.setUserId(userId);
                player.setUserId(userId);
                System.out.println("insert user count: " + cnt + ", user=" + user);
            }

            // insert player
            playerPreparedStatement.setInt(1, player.getUserId());
            playerPreparedStatement.setString(2, player.getPlayerName());
            playerPreparedStatement.setInt(3, player.getPlayerLevel());
            cnt = playerPreparedStatement.executeUpdate();
            resultSet = playerPreparedStatement.getGeneratedKeys();
            if (resultSet.next()) {
                int playerId = resultSet.getInt(1);
                player.setPlayerId(playerId);
                System.out.println("insert player count: " + cnt + ", player=" + player);
            }
        } catch (SQLException throwables) {
            throwables.printStackTrace();
        }

        return user.getUserId();
    }

    public static User QueryUser(Connection conn, String qryUserName) {
        User user = null;
        String sql = "select user_id, user_name, password, reg_date, reg_ip from t_user where user_name=?";
        try (PreparedStatement preparedStatement = conn.prepareStatement(sql)) { // try-with-resources Statement
            preparedStatement.setString(1, qryUserName);
            try (ResultSet resultSet = preparedStatement.executeQuery()) { // try-with-resources ResultSet
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
        String sqlDeleteUser = "delete from t_user where user_id = ?";
        String sqlDeletePlayer = "delete from t_user where user_id = ?";
        try (PreparedStatement userPreparedStatement = conn.prepareStatement(sqlDeleteUser);
             PreparedStatement playerPreparedStatement = conn.prepareStatement(sqlDeletePlayer)) { // try-with-resources Statement
            // delete user
            userPreparedStatement.setInt(1, user.getUserId());
            int cnt = userPreparedStatement.executeUpdate();
            System.out.println("delete user count: " + cnt);

            // delete player
            playerPreparedStatement.setInt(1, user.getUserId());
            cnt = playerPreparedStatement.executeUpdate();
            System.out.println("delete player count: " + cnt);
        } catch (SQLException throwables) {
            throwables.printStackTrace();
        }
    }
}
