#!/bin/bash

./stop.sh

mkdir data-dir
mkdir consul.d

nohup ./consul agent \
	-server \
	-bootstrap-expect=1 \
	-data-dir=$PWD/data-dir \
	-node=agent-one \
	-bind=127.0.0.1 \
	-enable-script-checks=true \
	-config-dir=$PWD/consul.d \
	-client=127.0.0.1 \
	&

