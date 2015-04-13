project(thread_pool)

set(SOURCES
    thread_pool.cpp
)

include_directories("${CMAKE_CURRENT_LIST_DIR}/../..")

add_library(thread_pool ${SOURCES})
