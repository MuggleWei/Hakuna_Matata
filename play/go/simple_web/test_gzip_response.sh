#!/bin/bash

curl \
	-v \
	--header "Content-Type: application/json" \
	-sH "Accept-encoding: gzip" \
	--request POST \
	--data 'hello world' \
	http://127.0.0.1:8080/api/v1/hello/echo \
	| gunzip -c
