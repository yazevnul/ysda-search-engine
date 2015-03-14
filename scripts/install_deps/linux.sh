#!/bin/bash -x

if [ ${CXX} == 'g++' ]; then
    sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
    sudo apt-get update -qq
    sudo apt-get install -y gcc-4.8 g++-4.8
    sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.8 20
    sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.8 20
    sudo update-alternatives --config gcc
    sudo update-alternatives --config g++

    g++ --version
elif [ ${CXX} == 'clang++' ]; then
    sudo apt-get update -qq
    sudo add-apt-repository 'deb http://llvm.org/apt/precise/ llvm-toolchain-precise main'
    wget -O - http://llvm.org/apt/llvm-snapshot.gpg.key | sudo apt-key add -
    sudo apt-get update -qq
    sudo apt-get install -y clang-3.5
fi

sudo apt-get install -y \
    protobuf-compiler   \
    libprotobuf-dev
