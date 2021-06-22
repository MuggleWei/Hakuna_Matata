CREATE DATABASE IF NOT EXISTS hello;
USE hello;

DROP TABLE IF EXISTS t_user;
CREATE TABLE IF NOT EXISTS t_user (
    id BIGINT NOT NULL AUTO_INCREMENT COMMENT 'user id',    -- 用户id
    name VARCHAR(64) NOT NULL COMMENT 'user name',          -- 用户名
    user_type INT NOT NULL DEFAULT 0 COMMENT 'user type',   -- 用户类型
    status INT NOT NULL COMMENT 'user status',              -- 用户状态
    PRIMARY KEY (id),
    UNIQUE INDEX UNIQUE_name (name)
) ENGINE=INNODB DEFAULT CHARSET=UTF8;
ALTER TABLE t_user AUTO_INCREMENT = 10001;

DROP TABLE IF EXISTS t_backup_user;
CREATE TABLE IF NOT EXISTS t_backup_user (
    id BIGINT NOT NULL AUTO_INCREMENT COMMENT 'user id',    -- 用户id
    name VARCHAR(64) NOT NULL COMMENT 'user name',          -- 用户名
    user_type INT NOT NULL DEFAULT 0 COMMENT 'user type',   -- 用户类型
    status INT NOT NULL COMMENT 'user status',              -- 用户状态
    PRIMARY KEY (id),
    UNIQUE INDEX UNIQUE_name (name)
) ENGINE=INNODB DEFAULT CHARSET=UTF8;