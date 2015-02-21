#!/bin/bash -e

if [ 'debug' = $1 ]; then
    clang++                                              \
        -std=c++11                                       \
        -stdlib=libc++                                   \
        -g                                               \
        -Wall                                            \
        -Wextra                                          \
        -pedantic                                        \
        -lprotobuf                                       \
        -I../../                                         \
        main.cpp                                         \
        ../../library/index/dictionary.pb.cc             \
        ../../library/protobuf_helpers/serialization.cpp \
        ../../library/index/index.cpp                    \
        ../../library/index/index.pb.cc                  \
        -o index_parser
elif [ 'release' = $1 ]; then
    clang++                                              \
        -std=c++11                                       \
        -stdlib=libc++                                   \
        -O2                                              \
        -Wall                                            \
        -Wextra                                          \
        -pedantic                                        \
        -lprotobuf                                       \
        -I../../                                         \
        main.cpp                                         \
        ../../library/index/dictionary.pb.cc             \
        ../../library/protobuf_helpers/serialization.cpp \
        ../../library/index/index.cpp                    \
        ../../library/index/index.pb.cc                  \
        -o index_parser
fi
