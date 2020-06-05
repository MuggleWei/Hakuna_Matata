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

CREATE TABLE t_player_asset (
	player_id INT NOT NULL COMMENT 'player_id in t_player',
    asset_type INT NOT NULL COMMENT 'asset type',
    asset_val INT DEFAULT 0
)ENGINE=InnoDB;