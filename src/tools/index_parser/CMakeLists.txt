project(index_parser)

set(SOURCES
    main.cpp
)

include_directories("${CMAKE_CURRENT_LIST_DIR}/../..")

add_executable(index_parser ${SOURCES})
target_link_libraries(index_parser
    ${PROTOBUF_LIBRARIES}
    index
    protobuf_helpers
)
