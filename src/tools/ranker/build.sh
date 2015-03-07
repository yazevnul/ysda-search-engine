#!/bin/bash -ex

if [ 'debug' = $1 ]; then
    clang++                                              \
        -std=c++1y                                       \
        -stdlib=libc++                                   \
        -g                                               \
        -Weverything                                     \
        -Wno-c++98-compat-pedantic                       \
        -Wno-padded                                      \
        -Wno-missing-prototypes                          \
        -fcolor-diagnostics                              \
        -lprotobuf                                       \
        -I../../                                         \
        main.cpp                                         \
        ../../library/index/dictionary.pb.cc             \
        ../../library/protobuf_helpers/serialization.cpp \
        ../../library/index/index.cpp                    \
        ../../library/index/index.pb.cc                  \
        -o ranker
elif [ 'release' = $1 ]; then
    clang++                                              \
        -std=c++1y                                       \
        -stdlib=libc++                                   \
        -mllvm                                           \
        -inline-threshold=10000                          \
        -O3                                              \
        -Weverything                                     \
        -Wno-c++98-compat-pedantic                       \
        -Wno-padded                                      \
        -Wno-missing-prototypes                          \
        -fcolor-diagnostics                              \
        -lprotobuf                                       \
        -I../../                                         \
        main.cpp                                         \
        ../../library/index/dictionary.pb.cc             \
        ../../library/protobuf_helpers/serialization.cpp \
        ../../library/index/index.cpp                    \
        ../../library/index/index.pb.cc                  \
        -o ranker
fi
