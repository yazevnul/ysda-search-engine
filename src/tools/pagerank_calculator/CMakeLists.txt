project(pagerank_calculator)

set(SOURCES
    main.cpp
)

include_directories("${CMAKE_CURRENT_LIST_DIR}/../..")

add_executable(pagerank_calculator ${SOURCES})
target_link_libraries(pagerank_calculator
    ${CMAKE_THREAD_LIBS_INIT}
)

