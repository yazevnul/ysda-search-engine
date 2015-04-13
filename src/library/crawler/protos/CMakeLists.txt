project(crawler_protos)

PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS simple_crawler_config.proto)

set(SOURCE
    simple_crawler_config.pb.cc
)

add_library(crawler_protos ${SOURCE})
target_link_libraries(crawler_protos
    ${PROTOBUF_LIBRARIES}
)
