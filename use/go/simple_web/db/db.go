package db

import (
	"time"

	_ "github.com/go-sql-driver/mysql"
	log "github.com/sirupsen/logrus"

	"xorm.io/xorm"
)

var (
	instance *xorm.Engine
)

func Instance() *xorm.Engine {
	return instance
}

func InitInstance(dsn string) error {
	engine, err := xorm.NewEngine("mysql", dsn)
	if err != nil {
		log.Fatal(err)
		return err
	}
	instance = engine

	// 设置最大连接数为
	instance.SetMaxOpenConns(64)

	// 设置最大空闲连接, MaxIdleConns应该始终小于或等于MaxOpenConns
	instance.SetMaxIdleConns(8)

	// 设置连接的最大生命周期为一小时。
	instance.SetConnMaxLifetime(time.Hour)

	return nil
}
