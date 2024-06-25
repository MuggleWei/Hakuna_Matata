#!/bin/bash

# handle argv
if [ "$#" -lt 1 ]; then
	echo "[ERROR] run.sh without app"
	echo "[ERROR] Usage: run.sh <app name>"
	exit 1
else
	app=$1
fi

streamlit run ./examples/$app/main.py \
    --server.address 0.0.0.0 \
    --server.port 8080 \
	--server.headless true
