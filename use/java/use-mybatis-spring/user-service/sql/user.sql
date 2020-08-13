CREATE DATABASE IF NOT EXISTS dc_db DEFAULT CHARACTER SET utf8;
USE dc_db;

DROP TABLE IF EXISTS t_user;
CREATE TABLE IF NOT EXISTS t_user (
    id BIGINT NOT NULL AUTO_INCREMENT COMMENT 'user id',
    name VARCHAR(64) NOT NULL COMMENT 'user name',
    password VARCHAR(64) NOT NULL COMMENT 'user password',
    status INT NOT NULL DEFAULT 0 COMMENT '0 - normal, 1 - freeze',
    reg_date datetime COMMENT 'register date',
    reg_ip VARCHAR(23) COMMENT 'register ip',
    update_date datetime COMMENT 'update date',
    PRIMARY KEY (id),
    UNIQUE INDEX UNIQUE_name (name)
)  ENGINE=INNODB AUTO_INCREMENT 10001 DEFAULT CHARSET=UTF8;