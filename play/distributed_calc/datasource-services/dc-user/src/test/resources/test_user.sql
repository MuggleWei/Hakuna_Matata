CREATE DATABASE IF NOT EXISTS test_dc_db DEFAULT CHARACTER SET utf8;
USE test_dc_db;

DROP TABLE IF EXISTS t_user;
CREATE TABLE IF NOT EXISTS t_user (
    id BIGINT NOT NULL AUTO_INCREMENT COMMENT 'user id',
    name VARCHAR(64) NOT NULL COMMENT 'user name',
    password VARCHAR(64) NOT NULL COMMENT 'user password',
    status INT NOT NULL DEFAULT 0 COMMENT '0 - normal, 1 - prohibited',
    PRIMARY KEY (id),
    UNIQUE INDEX UNIQUE_name (name)
)  ENGINE=INNODB AUTO_INCREMENT 10001 DEFAULT CHARSET=UTF8;

DROP TABLE IF EXISTS t_authority;
CREATE TABLE IF NOT EXISTS t_authority (
    user_id BIGINT NOT NULL COMMENT 'user id',
    num_tasks INT NOT NULL DEFAULT 1 COMMENT 'allow user to run how many tasks at the same time',
    PRIMARY KEY (user_id)
) ENGINE=INNODB DEFAULT CHARSET=UTF8;

-- add test datas
INSERT INTO t_user (name, password, status) VALUES
('test_user1', '123456abcABC', 0),
('test_user2', '123456abcABC', 1);

INSERT INTO t_authority (user_id, num_tasks) VALUES
(10001, 1),
(10002, 100);