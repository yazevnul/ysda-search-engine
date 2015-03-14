#!/bin/bash -x

## GCC-4.8 installation BEGIN

sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
sudo apt-get update -qq
sudo apt-get install -y gcc-4.8 g++-4.8
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.8 20
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.8 20

which g++
g++ --version

## GCC-4.8 installation END

if [ ${CXX} == 'clang++' ]; then
    which clang++

    sudo apt-get update -qq
    sudo add-apt-repository -y 'deb http://llvm.org/apt/precise/ llvm-toolchain-precise-3.5 main'
    wget -O - http://llvm.org/apt/llvm-snapshot.gpg.key | sudo apt-key add -
    sudo apt-get update -qq
    sudo apt-get install -y     \
        clang-3.5               \
        libclang-3.5-dev        \
        libllvm-3.5-ocaml-dev   \
        libclang-common-3.5-dev \
        llvm-3.5-dev            \
        libobjc-4.8-dev         \
        libc++1
    sudo apt-get autoremove -y libgeos-3.2.2
    sudo update-alternatives --install /usr/bin/clang clang /usr/lib/llvm-3.5/bin/clang 20
    sudo update-alternatives --install /usr/bin/clang++ clang++ /usr/lib/llvm-3.5/bin/clang++ 20

    which clang++
    clang++ --version
fi

sudo apt-get install -y \
    protobuf-compiler   \
    libprotobuf-dev
