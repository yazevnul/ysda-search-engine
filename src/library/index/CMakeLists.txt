project(index)

add_subdirectory(protos)

set(SOURCE
    index.cpp
)

include_directories("${CMAKE_CURRENT_LIST_DIR}/../..")

add_library(index ${SOURCE})
target_link_libraries(index
    ${PROTOBUF_LIBRARIES}
    protos
)
