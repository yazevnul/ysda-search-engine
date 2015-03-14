#!/bin/bash -x

sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
sudo apt-get update -qq
sudo apt-get install -y gcc-4.8

sudo apt-get install -y \
    protobuf-compiler   \
    libprotobuf-dev

sudo apdate-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.8 50
