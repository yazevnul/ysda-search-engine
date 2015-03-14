project(protos)

PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS index.proto)
PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS dictionary.proto)

set(SOURCE
    index.pb.cc
    dictionary.pb.cc
)

add_library(protos ${SOURCE})
target_link_libraries(protos
    ${PROTOBUF_LIBRARIES}
)
