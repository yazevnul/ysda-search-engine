project(protobuf_helpers_lib)

set(SOURCES
    serialization.cpp
)

add_library(protobuf_helpers ${SOURCES})
target_link_libraries(protobuf_helpers
    ${PROTOBUF_LIBRARIES}
)
