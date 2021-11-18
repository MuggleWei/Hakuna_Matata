CREATE DATABASE IF NOT EXISTS hello;
USE hello;

-- 用户表
DROP TABLE IF EXISTS t_user;
CREATE TABLE IF NOT EXISTS t_user (
    id BIGINT NOT NULL AUTO_INCREMENT COMMENT 'user id',    -- 用户id
    name VARCHAR(64) NOT NULL COMMENT 'user name',          -- 用户名
    user_type INT NOT NULL DEFAULT 0 COMMENT 'user type',   -- 用户类型
    status INT NOT NULL COMMENT 'user status',              -- 用户状态
    PRIMARY KEY (id),
    UNIQUE INDEX UNIQUE_name (name)
) ENGINE=INNODB DEFAULT CHARSET=UTF8 COLLATE utf8_bin;
ALTER TABLE t_user AUTO_INCREMENT = 10001;

-- 备份用户表(为了可以在同一个数据库中跑测试, 所以增加一个这张表)
DROP TABLE IF EXISTS t_backup_user;
CREATE TABLE IF NOT EXISTS t_backup_user (
    id BIGINT NOT NULL AUTO_INCREMENT COMMENT 'user id',    -- 用户id
    name VARCHAR(64) NOT NULL COMMENT 'user name',          -- 用户名
    user_type INT NOT NULL DEFAULT 0 COMMENT 'user type',   -- 用户类型
    status INT NOT NULL COMMENT 'user status',              -- 用户状态
    PRIMARY KEY (id),
    UNIQUE INDEX UNIQUE_name (name)
) ENGINE=INNODB DEFAULT CHARSET=UTF8 COLLATE utf8_bin;

-- 资金流水表
DROP TABLE IF EXISTS t_fund_stream_record;
CREATE TABLE IF NOT EXISTS t_fund_stream_record (
	id BIGINT NOT NULL COMMENT 'fund stream record id',  -- 资金流水id
	user_id BIGINT NOT NULL COMMENT 'user id',           -- 用户id
	dir INT NOT NULL COMMENT 'fund stream dir',          -- 资金流水方向
	amount DECIMAL(20, 2) NOT NULL COMMENT 'amount',     -- 资金量
	PRIMARY KEY (id),
	INDEX INDEX_user (user_id)
) ENGINE=INNODB DEFAULT CHARSET=UTF8 COLLATE utf8_bin;
