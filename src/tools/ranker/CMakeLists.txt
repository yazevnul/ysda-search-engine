project(ranker)

set(SOURCES
    main.cpp
)

include_directories("${CMAKE_CURRENT_LIST_DIR}/../..")

add_executable(ranker ${SOURCES})
target_link_libraries(ranker
    ${PROTOBUF_LIBRARIES}
    index
    protobuf_helpers
)
