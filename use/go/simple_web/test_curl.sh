#!/bin/bash

echo "/api/v1/album/query?title=xxx"
curl http://127.0.0.1:10102/api/v1/album/query?title=xxx
echo ""

echo "/api/v1/album/query?title=Blue+Train"
curl http://127.0.0.1:10102/api/v1/album/query?title=Blue+Train
echo ""

echo "/api/v1/album/query?artist=John+Coltrane"
curl http://127.0.0.1:10102/api/v1/album/query?artist=John+Coltrane
echo ""

echo "/api/v1/album/query_by_price '{\"lower\": 0.0, \"upper\": 50.0}'"
curl \
	--header "Content-Type: application/json" \
	--request POST \
	--data '{"lower": 0.0, "upper": 50.0}' \
	http://127.0.0.1:10102/api/v1/album/query_by_price
echo ""

