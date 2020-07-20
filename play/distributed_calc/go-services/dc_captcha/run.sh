#!/bin/bash

killall dc_captcha

host=127.0.0.1
port=10102
consul_host=$host

nohup ./dc_captcha \
	-host=$host \
	-port=$port \
	-redis.host=$host \
	-redis.port=6379 \
	-consul.host=$consul_host \
	-consul.port=8500 \
	-consul.service=dc_captcha \
	-consul.id=dc_captcha-1 \
	-consul.ttl=30s \
	&
