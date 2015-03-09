project(protobuf_helpers_lib)

set(HEADERS
    serialization.h
)

set(SOURCES
    serialization.cpp
)

add_library(protobuf_helpers ${SOURCES} ${HEADERS})
target_link_libraries(protobuf_helpers
    ${PROTOBUF_LIBRARIES}
)
