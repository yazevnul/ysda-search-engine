#!/bin/bash -x

if [ ${CXX} = 'g++' ]; then
    sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
    sudo apt-get update -qq
    sudo apt-get install -y gcc-4.8
    sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.8 50
elif [ ${GXX} = 'clang++' ]; then
    sudo deb http://llvm.org/apt/precise/ llvm-toolchain-precise-3.5 main
    sudo deb-src http://llvm.org/apt/precise/ llvm-toolchain-precise-3.5 main
    wget -O - http://llvm.org/apt/llvm-snapshot.gpg.key | sudo apt-key add -
    sudo apt-get install -y clang-3.5
fi

sudo apt-get install -y \
    protobuf-compiler   \
    libprotobuf-dev
