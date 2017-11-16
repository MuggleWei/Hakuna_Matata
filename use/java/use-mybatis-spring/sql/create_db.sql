create database if not exists systemdb;
use systemdb;

-- create account_info table
drop table if exists account_info;
CREATE TABLE IF NOT EXISTS account_info (
    id BIGINT NOT NULL AUTO_INCREMENT,
    phone VARCHAR(32),
    mail VARCHAR(32),
    name VARCHAR(32),
    PRIMARY KEY (id)
)  ENGINE=INNODB DEFAULT CHARSET=UTF8;
CREATE UNIQUE INDEX INDEX_phone_NAME ON account_info (phone);
CREATE UNIQUE INDEX INDEX_mail_NAME ON account_info (mail);

-- create account_passwd table
drop table if exists account_passwd;
create table if not exists account_passwd(
	id BIGINT NOT NULL,
    passwd VARCHAR(64),
    old1 VARCHAR(64),
    old2 VARCHAR(64),
    old3 VARCHAR(64),
    PRIMARY KEY (id)
)ENGINE=InnoDB;

-- add init data
insert into account_info (id, name) values (10000, 'admin');
insert into account_info (name) values
('default1'),('default2'),('default3'),('default4'),('default5'),
('default6'),('default7'),('default8'),('default9'),('default10'),
('default11'),('default12'),('default13'),('default14'),('default15'),
('default16'),('default17'),('default18'),('default19'),('default20');

insert into account_passwd (id, passwd) values (10000, '123'),
(10001, '123'),(10002, '123'),(10003, '123'),(10004, '123'),(10005, '123'),
(10006, '123'),(10007, '123'),(10008, '123'),(10009, '123'),(10010, '123'),
(10011, '123'),(10012, '123'),(10013, '123'),(10014, '123'),(10015, '123'),
(10016, '123'),(10017, '123'),(10018, '123'),(10019, '123'),(10020, '123');
