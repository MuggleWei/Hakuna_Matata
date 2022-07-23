package db

import (
	"errors"
	"fmt"
	"time"

	_ "github.com/go-sql-driver/mysql"
	log "github.com/sirupsen/logrus"

	"xorm.io/xorm"
)

var (
	instanceMap map[string]*xorm.Engine
)

func GetInstance(name string) (*xorm.Engine, error) {
	engine, ok := instanceMap[name]
	if !ok {
		errMsg := fmt.Sprintf("Db GetInstance not exists: %v", name)
		err := errors.New(errMsg)
		log.Error(err.Error())
		return nil, err
	} else {
		return engine, nil
	}
}

func InitInstance(dbMap map[string]string) error {
	instanceMap = make(map[string]*xorm.Engine)

	for name, dsn := range dbMap {
		engine, err := xorm.NewEngine("mysql", dsn)
		if err != nil {
			log.Fatal(err)
			return err
		}

		// 设置最大连接数为
		engine.SetMaxOpenConns(64)

		// 设置最大空闲连接, MaxIdleConns应该始终小于或等于MaxOpenConns
		engine.SetMaxIdleConns(8)

		// 设置连接的最大生命周期为一小时。
		engine.SetConnMaxLifetime(time.Hour)

		instanceMap[name] = engine
	}

	return nil
}
