DROP DATABASE IF EXISTS sample_db;
CREATE DATABASE sample_db DEFAULT CHARACTER SET utf8;
USE sample_db;

CREATE TABLE t_user (
   user_id INT NOT NULL AUTO_INCREMENT COMMENT 'user id',
   user_name VARCHAR(30) NOT NULL COMMENT 'user name',
   password  VARCHAR(32) NOT NULL COMMENT 'password',
   reg_date datetime COMMENT 'register date',
   reg_ip VARCHAR(23) COMMENT 'register ip',
   PRIMARY KEY (user_id)
)ENGINE=InnoDB;

CREATE TABLE t_player (
	player_id INT NOT NULL AUTO_INCREMENT COMMENT 'player id',
	user_id INT NOT NULL COMMENT 'user_id in table t_user',
    region VARCHAR(32) NOT NULL COMMENT 'server region',
	player_name VARCHAR(32) NOT NULL COMMENT 'player name',
    player_level INT DEFAULT 0 COMMENT 'player level',
    PRIMARY KEY (player_id)
)ENGINE=InnoDB;
CREATE UNIQUE INDEX INDEX_user_region ON t_player (user_id, region);
CREATE UNIQUE INDEX INDEX_region_player ON t_player (region, player_name);

begin;

insert into t_user (user_name, password, reg_date, reg_ip) values
("testuser1", "123456", CURDATE(), "127.0.0.1"),
("testuser2", "123456", CURDATE(), "127.0.0.1"),
("testuser3", "123456", CURDATE(), "127.0.0.1"),
("testuser4", "123456", CURDATE(), "127.0.0.1"),
("testuser5", "123456", CURDATE(), "127.0.0.1"),
("testuser6", "123456", CURDATE(), "127.0.0.1"),
("testuser7", "123456", CURDATE(), "127.0.0.1"),
("testuser8", "123456", CURDATE(), "127.0.0.1"),
("testuser9", "123456", CURDATE(), "127.0.0.1"),
("testuser10", "123456", CURDATE(), "127.0.0.1");

insert into t_player (user_id, region, player_name) values
(1, "Azeroth", "testuser1"),
(2, "Azeroth", "testuser2"),
(3, "Azeroth", "testuser3"),
(4, "Azeroth", "testuser4"),
(5, "Azeroth", "testuser5"),
(6, "Azeroth", "testuser6"),
(7, "Azeroth", "testuser7"),
(8, "Azeroth", "testuser8"),
(9, "Azeroth", "testuser9"),
(10, "Azeroth", "testuser10");

commit;