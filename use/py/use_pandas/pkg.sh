#!/bin/bash

origin_dir="$(dirname "$(readlink -f "$0")")"
cd $origin_dir

if [ ! -d "venv" ]; then
    python -m venv venv
fi

source venv/bin/activate
pip install -r requirements.dev.txt
mkdir -p dist

pyinstaller \
    -p venv/Lib/site-packages \
    -F examples/concat_column/concat_column.py \
    -n concat_column \
    --distpath dist/concat_column/bin

pyinstaller \
    -p venv/Lib/site-packages \
    -F examples/add_column/add_column.py \
    -n add_column \
    --distpath dist/add_column/bin

cp -r datas dist/
