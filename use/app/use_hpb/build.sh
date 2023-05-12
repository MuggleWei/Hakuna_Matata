#!/bin/bash

hpb build -m task -c modules/mugglec.yml -p build_type=debug
hpb build -m task -c modules/mugglec.yml -p build_type=release

hpb build -m task -c modules/mugglecpp.yml -p build_type=debug
hpb build -m task -c modules/mugglecpp.yml -p build_type=release
