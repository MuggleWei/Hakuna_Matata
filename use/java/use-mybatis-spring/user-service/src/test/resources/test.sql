CREATE DATABASE IF NOT EXISTS sample_db DEFAULT CHARACTER SET utf8;
USE sample_db;

DROP TABLE IF EXISTS t_user;
CREATE TABLE IF NOT EXISTS t_user (
    id BIGINT NOT NULL AUTO_INCREMENT COMMENT 'user id',
    name VARCHAR(64) NOT NULL COMMENT 'user name',
    password VARCHAR(64) NOT NULL COMMENT 'user password',
    status INT NOT NULL DEFAULT 0 COMMENT 'user status',
    reg_date datetime COMMENT 'register date',
    reg_ip VARCHAR(23) COMMENT 'register ip',
    update_date datetime COMMENT 'update date',
    PRIMARY KEY (id),
    UNIQUE INDEX UNIQUE_name (name)
)  ENGINE=INNODB AUTO_INCREMENT 10001 DEFAULT CHARSET=UTF8;

DROP TABLE IF EXISTS t_level;
CREATE TABLE IF NOT EXISTS t_level (
    id BIGINT NOT NULL COMMENT 'user id',
    level INT NOT NULL DEFAULT 1 COMMENT 'user level',
    PRIMARY KEY (id)
) ENGINE=INNODB DEFAULT CHARSET=UTF8;

insert into t_user (name, password, status, reg_date, reg_ip, update_date) values
("testuser1", "123456", 0, now(), "127.0.0.1", now()),
("testuser2", "123456", 0, now(), "127.0.0.1", now()),
("testuser3", "123456", 0, now(), "127.0.0.1", now());