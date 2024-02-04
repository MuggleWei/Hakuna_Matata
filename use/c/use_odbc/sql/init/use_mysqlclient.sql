CREATE DATABASE IF NOT EXISTS testdb CHARACTER SET utf8mb4 COLLATE utf8mb4_bin;
USE testdb;

SET NAMES 'utf8mb4';

DROP TABLE IF EXISTS students;
CREATE TABLE IF NOT EXISTS students (
	`id` int NOT NULL AUTO_INCREMENT,
	`name` VARCHAR(32) NOT NULL,
	`email` VARCHAR(64) NOT NULL,
	PRIMARY KEY (`id`),
	UNIQUE INDEX idx_email (email),
	INDEX idx_name (name)
) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin;

INSERT INTO students (name, email)
VALUES
	('foo', "foo@example.com"),
	('bar', "bar@example.com"),
	('baz', "baz@example.com"),
	('小明', "xiaoming@example.com"),
	('小李', "xiaoli@example.com"),
	('小白', "xiaobai@example.com");
