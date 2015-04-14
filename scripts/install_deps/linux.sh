#!/bin/bash -x

## GCC-4.9 installation BEGIN

sudo add-apt-repository --yes ppa:ubuntu-toolchain-r/test
sudo apt-get update -qq
sudo apt-get install --yes gcc-4.9 g++-4.9
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.9 20
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.9 20

## GCC-4.9 installation END

if [ ${CXX} == 'clang++' ]; then
    sudo apt-get update -qq
    sudo add-apt-repository --yes 'deb http://llvm.org/apt/precise/ llvm-toolchain-precise-3.6 main'
    wget --output-document - http://llvm.org/apt/llvm-snapshot.gpg.key | sudo apt-key add -
    sudo apt-get update -qq
    sudo apt-get install --yes  \
        clang-3.6               \
        libclang-3.6-dev        \
        libllvm-3.6-ocaml-dev   \
        libclang-common-3.6-dev \
        llvm-3.6-dev            \
        libobjc-4.8-dev
    sudo update-alternatives --install /usr/bin/clang clang /usr/lib/llvm-3.6/bin/clang 20
    sudo update-alternatives --install /usr/bin/clang++ clang++ /usr/lib/llvm-3.6/bin/clang++ 20

    sudo rm /usr/local/clang-3.4/bin/clang++
fi

sudo apt-get install --yes \
    protobuf-compiler      \
    libprotobuf-dev
