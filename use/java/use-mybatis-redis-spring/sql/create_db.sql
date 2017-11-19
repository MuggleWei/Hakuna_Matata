create database if not exists account;
use account;

-- create account_info table
drop table if exists account_info;
CREATE TABLE IF NOT EXISTS account_info (
    id BIGINT NOT NULL AUTO_INCREMENT,
    phone VARCHAR(32),
    mail VARCHAR(32),
    name VARCHAR(32),
    PRIMARY KEY (id)
)  ENGINE=INNODB DEFAULT CHARSET=UTF8;
CREATE UNIQUE INDEX INDEX_phone ON account_info (phone);
CREATE UNIQUE INDEX INDEX_mail ON account_info (mail);

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

-- create trade table
drop table if exists trade_record;
CREATE TABLE IF NOT EXISTS trade_record (
    id VARCHAR(64) NOT NULL,
    account_id BIGINT NOT NULL,
    product_id VARCHAR(64) NOT NULL,
    price DOUBLE NOT NULL,
    trade_time BIGINT NOT NULL,
    trade_date DATE NOT NULL,
    PRIMARY KEY (id)
)  ENGINE=INNODB;
CREATE UNIQUE INDEX INDEX_id ON trade_record (id);
CREATE INDEX INDEX_trade_date ON trade_record (trade_date);
CREATE INDEX INDEX_product_id ON trade_record (product_id);