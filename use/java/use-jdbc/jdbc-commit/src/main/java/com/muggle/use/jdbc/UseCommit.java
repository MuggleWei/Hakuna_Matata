package com.muggle.use.jdbc;

import java.sql.*;

public class UseCommit {
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
        Connection conn = null;
        try {
            conn = DriverManager.getConnection(URL, USER, PASSWORD);
        } catch (SQLException throwables) {
            throwables.printStackTrace();
            return;
        }

        try {
            // 关闭自动提交
            conn.setAutoCommit(false);

            // TODO:

            // 手动提交
            conn.commit();
        } catch (SQLException throwables) {
            throwables.printStackTrace();
            try {
                System.out.println("rollback");
                conn.rollback();
            } catch (SQLException e) {
                e.printStackTrace();
            }
        }
    }
}
