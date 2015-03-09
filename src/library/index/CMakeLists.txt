project(index)

PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS index.proto)
PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS dictionary.proto)

set(HEADERS
    dictionary.pb.h
    index.h
    index.pb.h
)

set(SOURCE
    index.cpp
    index.pb.cc
    dictionary.pb.cc
)

include_directories("${CMAKE_CURRENT_LIST_DIR}/../..")

add_library(index ${SOURCE} ${HEADERS})
target_link_libraries(index
    ${PROTOBUF_LIBRARIES}
)
