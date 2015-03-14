#!/bin/bash -x

sudo apt-get update -qq
sudo apt-get install -y \
    protobuf-compiler   \
    libprotobuf-dev
    g++-4.8

if [ "${CXX}" == 'g++' ]; then
    export CXX='g++-4.8'
    export CC='gcc-4.8'
fi
