#!/bin/bash

echo "/api/v1/hello/echo 'hello world'"
curl \
	--header "Content-Type: application/json" \
	--request POST \
	--data 'hello world' \
	http://127.0.0.1:8080/api/v1/hello/echo
echo ""
echo ""

echo "/api/v1/hello/greet '{\"name\": \"muggle\"}'"
curl \
	--header "Content-Type: application/json" \
	--request POST \
	--data '{"name": "muggle"}' \
	http://127.0.0.1:8080/api/v1/hello/greet
echo ""
echo ""

echo "error example /api/v1/hello/greet '{}'"
curl \
	--header "Content-Type: application/json" \
	--request POST \
	--data '{}' \
	http://127.0.0.1:8080/api/v1/hello/greet
echo ""
echo ""

echo "/api/v1/album/query?title=xxx"
curl http://127.0.0.1:8080/api/v1/album/query?title=xxx
echo ""
echo ""

echo "/api/v1/album/query?title=Blue+Train"
curl http://127.0.0.1:8080/api/v1/album/query?title=Blue+Train
echo ""
echo ""

echo "/api/v1/album/query?artist=John+Coltrane"
curl http://127.0.0.1:8080/api/v1/album/query?artist=John+Coltrane
echo ""
echo ""

echo "/api/v1/album/query_by_price '{\"lower\": 0.0, \"upper\": 50.0}'"
curl \
	--header "Content-Type: application/json" \
	--request POST \
	--data '{"lower": 0.0, "upper": 50.0}' \
	http://127.0.0.1:8080/api/v1/album/query_by_price
echo ""
echo ""
