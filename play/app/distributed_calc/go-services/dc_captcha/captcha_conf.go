package main

import (
	"flag"
	"time"
)

type CaptchaConfig struct {
	// service
	Host string
	Port int

	// redis
	RedisHost   string
	RedisPort   int
	RedisPasswd string
	RedisDB     int

	// consul
	ConsulHost    string
	ConsulPort    int
	ConsulService string
	ConsulID      string
	ConsulTag     string
	ConsulTTL     time.Duration

	// captcha
	ExpireSecond int
}

func LoadCaptchaConfig() (*CaptchaConfig, error) {
	host := flag.String("host", "127.0.0.1", "bind host")
	port := flag.Int("port", 10102, "listen port")

	redisHost := flag.String("redis.host", "127.0.0.1", "redis host")
	redisPort := flag.Int("redis.port", 6379, "redis port")
	redisPasswd := flag.String("redis.passwd", "", "redis password")
	redisDB := flag.Int("redis.db", 0, "redis db")

	consulHost := flag.String("consul.host", "127.0.0.1", "consul host")
	consulPort := flag.Int("consul.port", 8500, "consul port")
	consulService := flag.String("consul.service", "captcha-service", "service name")
	consulID := flag.String("consul.id", "captcha-service-0", "service id")
	consulTag := flag.String("consul.tag", "", "consul service tags")
	consulTTL := flag.String("consul.ttl", "3s", "consul ttl")

	expireSecond := flag.Int("captcha.expire", 60, "captcha expire time (in seconds)")

	flag.Parse()

	ttl, err := time.ParseDuration(*consulTTL)
	if err != nil {
		return nil, err
	}

	return &CaptchaConfig{
		Host: *host,
		Port: *port,

		RedisHost:   *redisHost,
		RedisPort:   *redisPort,
		RedisPasswd: *redisPasswd,
		RedisDB:     *redisDB,

		ConsulHost:    *consulHost,
		ConsulPort:    *consulPort,
		ConsulService: *consulService,
		ConsulID:      *consulID,
		ConsulTag:     *consulTag,
		ConsulTTL:     ttl,

		ExpireSecond: *expireSecond,
	}, nil
}
