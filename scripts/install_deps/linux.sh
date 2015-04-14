#!/bin/bash -x

## GCC-5 installation BEGIN

sudo add-apt-repository --yes ppa:ubuntu-toolchain-r/test
sudo apt-get update -qq
sudo apt-get install --yes \
    g++-5                  \
    gcc-5
sudo apt-get install --fix-broken --yes
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-5 20
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-5 20

which g++
g++ --version

## GCC-5 installation END

if [ ${CXX} == 'clang++' ]; then
    sudo apt-get update -qq
    sudo add-apt-repository --yes 'deb http://llvm.org/apt/precise/ llvm-toolchain-precise-3.6 main'
    wget --output-document - 'http://llvm.org/apt/llvm-snapshot.gpg.key' | sudo apt-key add -
    sudo apt-get update -qq
    sudo apt-get install --yes     \
        clang-3.6                  \
        libclang-3.6-dev           \
        libllvm-3.6-ocaml-dev      \
        libclang-common-3.6-dev    \
        llvm-3.6-dev               \
        libobjc-4.8-dev
    sudo apt-get autoremove --yes libgeos-3.2.2
    sudo update-alternatives --install /usr/bin/clang clang /usr/lib/llvm-3.6/bin/clang 20
    sudo update-alternatives --install /usr/bin/clang++ clang++ /usr/lib/llvm-3.6/bin/clang++ 20

    sudo rm /usr/local/clang-3.4/bin/clang++

    which clang++
    clang++ --version
fi

sudo apt-get install --yes \
    protobuf-compiler      \
    libprotobuf-dev
