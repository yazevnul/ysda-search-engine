project(index_protos)

PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS index.proto)
PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS dictionary.proto)

set(SOURCE
    index.pb.cc
    dictionary.pb.cc
)

add_library(index_protos ${SOURCE})
target_link_libraries(index_protos
    ${PROTOBUF_LIBRARIES}
)
