package main

import (
	"context"
	"strconv"

	dc_common "github.com/MuggleWei/Hakuna_Matata/play/distributed_calc/go-services/dc_common"
	clb "github.com/MuggleWei/go-toy/clb"
	srd "github.com/MuggleWei/go-toy/srd"
	redis "github.com/go-redis/redis/v8"
)

type CaptchaComponents struct {
	ServiceDiscoveryClient srd.ServiceDiscoveryClient
	ClientLB               *clb.ClientLoadBalancer
	RedisClient            *redis.Client
	RedisCtx               context.Context
	RestUtils              *dc_common.RestUtils
}

func initComponents(config *CaptchaConfig) (*CaptchaComponents, error) {
	// init consul discovery client
	serviceDiscoveryClient, err := initServiceDiscoveryClient(config, nil)
	if err != nil {
		return nil, err
	}

	// init client load balancer
	clientLB := clb.NewClientLoadBalancer(serviceDiscoveryClient, config.ConsulTTL)

	// init redis
	ctx := context.Background()
	redisClient, err := initRedis(config, ctx)
	if err != nil {
		return nil, err
	}

	components := &CaptchaComponents{
		ServiceDiscoveryClient: serviceDiscoveryClient,
		ClientLB:               clientLB,
		RedisClient:            redisClient,
		RedisCtx:               ctx,
	}
	return components, nil
}

func initServiceDiscoveryClient(conf *CaptchaConfig, check func() (bool, error)) (srd.ServiceDiscoveryClient, error) {
	consulAddr := conf.ConsulHost + ":" + strconv.Itoa(conf.ConsulPort)
	serviceDiscoveryClient, err := srd.NewConsulClient(consulAddr)
	if err != nil {
		return nil, err
	}

	registration := srd.ServiceRegistration{
		ID:    conf.ConsulID,
		Name:  conf.ConsulService,
		Addr:  conf.Host,
		Port:  conf.Port,
		Tag:   []string{conf.ConsulTag},
		TTL:   conf.ConsulTTL,
		Check: check,
	}
	err = serviceDiscoveryClient.Register(&registration)
	if err != nil {
		return nil, err
	}

	return serviceDiscoveryClient, nil
}

func initRedis(conf *CaptchaConfig, ctx context.Context) (*redis.Client, error) {
	redisAddr := conf.RedisHost + ":" + strconv.Itoa(conf.RedisPort)
	redisClient := redis.NewClient(&redis.Options{
		Addr:     redisAddr,
		Password: conf.RedisPasswd,
		DB:       conf.RedisDB,
	})

	_, err := redisClient.Ping(ctx).Result()
	if err != nil {
		return nil, err
	}

	return redisClient, nil
}
