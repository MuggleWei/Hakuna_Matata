drop database hello;
create database if not exists hello;
use hello;

-- create account balance template
drop table if exists balance_template;
CREATE TABLE IF NOT EXISTS balance_template (
    id BIGINT,
    btc DOUBLE,
    usd DOUBLE,
    cny DOUBLE,
    PRIMARY KEY (id)
)  ENGINE=INNODB DEFAULT CHARSET=UTF8;
