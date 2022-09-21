-- ----------------
-- create databases
-- ----------------
CREATE DATABASE IF NOT EXISTS hello; 
USE hello;

-- ----------------
-- user table
-- ----------------
DROP TABLE IF EXISTS t_user;
CREATE TABLE t_user (
	id INT AUTO_INCREMENT NOT NULL COMMENT 'id',
	name VARCHAR(64) NOT NULL COMMENT 'user name',
	passwd VARCHAR(128) NOT NULL COMMENT 'user password bcrypt hash',
	PRIMARY KEY (`id`),
	UNIQUE INDEX unique_name(`name`)
) ENGINE=INNODB DEFAULT CHARSET=UTF8 COLLATE utf8_bin;

