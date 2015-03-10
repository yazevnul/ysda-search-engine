project(ranker)

set(SOURCES
    main.cpp
)

include_directories("${CMAKE_CURRENT_LIST_DIR}/../..")

add_executable(ranker ${SOURCES})
target_link_libraries(ranker
    ${PROTOBUF_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    index
    protobuf_helpers
)
