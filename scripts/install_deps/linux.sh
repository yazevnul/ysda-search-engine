#!/bin/bash -x

## GCC-5 installation BEGIN

sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
sudo apt-get update -qq
sudo apt-get install -y \
    binutils            \
    cpp-5               \
    g++-5               \
    gcc-5               \
    libc6               \
    libc6-dev           \
    libcc1-0            \
    libgcc-5-dev        \
    libgcc1             \
    libgmp10            \
    libisl10            \
    libmpc2             \
    libmpfr4            \
    libstdc++-5-dev     \
    libstdc++6          \
    zlib1g
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-5 20
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-5 20

which g++
g++ --version

## GCC-5 installation END

if [ ${CXX} == 'clang++' ]; then
    sudo apt-get update -qq
    sudo add-apt-repository -y 'deb http://llvm.org/apt/precise/ llvm-toolchain-precise-3.6 main'
    wget -O - http://llvm.org/apt/llvm-snapshot.gpg.key | sudo apt-key add -
    sudo apt-get update -qq
    sudo apt-get install -y     \
        clang-3.6               \
        libclang-3.6-dev        \
        libllvm-3.6-ocaml-dev   \
        libclang-common-3.6-dev \
        llvm-3.6-dev            \
        libobjc-4.8-dev
    sudo apt-get autoremove -y libgeos-3.2.2
    sudo update-alternatives --install /usr/bin/clang clang /usr/lib/llvm-3.6/bin/clang 20
    sudo update-alternatives --install /usr/bin/clang++ clang++ /usr/lib/llvm-3.6/bin/clang++ 20

    sudo rm /usr/local/clang-3.4/bin/clang++

    which clang++
    clang++ --version
fi

sudo apt-get install -y \
    protobuf-compiler   \
    libprotobuf-dev
