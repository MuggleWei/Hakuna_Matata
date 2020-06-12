DROP DATABASE IF EXISTS sample_db;
CREATE DATABASE sample_db DEFAULT CHARACTER SET utf8;
USE sample_db;

CREATE TABLE t_user (
   user_id BIGINT NOT NULL AUTO_INCREMENT COMMENT 'user id',
   user_name VARCHAR(30) NOT NULL COMMENT 'user name',
   reg_date datetime COMMENT 'register date',
   reg_ip VARCHAR(23) COMMENT 'register ip',
   PRIMARY KEY (user_id)
)ENGINE=InnoDB;

CREATE TABLE t_fund (
	user_id BIGINT NOT NULL COMMENT 'user_id in table t_user',
    currency_type INT NOT NULL COMMENT 'currency type',
	amount BIGINT NOT NULL COMMENT 'cash amount'
)ENGINE=InnoDB;
CREATE UNIQUE INDEX INDEX_user_currency ON t_fund (user_id, currency_type);

INSERT INTO t_user (user_name, reg_date, reg_ip) values
("testuser1", now(), "127.0.0.1"),
("testuser2", now(), "127.0.0.1"),
("testuser3", now(), "127.0.0.1"),
("testuser4", now(), "127.0.0.1"),
("testuser5", now(), "127.0.0.1"),
("testuser6", now(), "127.0.0.1"),
("testuser7", now(), "127.0.0.1"),
("testuser8", now(), "127.0.0.1"),
("testuser9", now(), "127.0.0.1"),
("testuser10", now(), "127.0.0.1");

INSERT INTO t_fund (user_id, currency_type, amount) values
(1, 0, 1000000),
(1, 1, 1000000),
(2, 0, 1000000),
(2, 1, 1000000),
(3, 0, 1000000),
(3, 1, 1000000),
(4, 0, 1000000),
(4, 1, 1000000),
(5, 0, 1000000),
(5, 1, 1000000),
(6, 0, 1000000),
(6, 1, 1000000),
(7, 0, 1000000),
(7, 1, 1000000),
(8, 0, 1000000),
(8, 1, 1000000),
(9, 0, 1000000),
(9, 1, 1000000),
(10, 0, 1000000),
(10, 1, 1000000);