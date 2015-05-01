project(graph_statistics)

set(SOURCES
    main.cpp
)

include_directories("${CMAKE_CURRENT_LIST_DIR}/../..")

add_executable(graph_statistics ${SOURCES})
target_link_libraries(graph_statistics
    ${CMAKE_THREAD_LIBS_INIT}
)
