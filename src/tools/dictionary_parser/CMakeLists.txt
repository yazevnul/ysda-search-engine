project(dictionary_parser)

set(SOURCES
    main.cpp
)

include_directories("${CMAKE_CURRENT_LIST_DIR}/../..")

add_executable(dictionary_parser ${SOURCES})
target_link_libraries(dictionary_parser
    ${PROTOBUF_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    index
    protobuf_helpers
)
