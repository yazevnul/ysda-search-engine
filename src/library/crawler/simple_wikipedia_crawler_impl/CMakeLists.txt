project(simple_wikipedia_crawler_impl)

include_directories("${CMAKE_CURRENT_LIST_DIR}/../../..")

set(SOURCES
    delete_me.cpp
)

add_library(simple_wikipedia_crawler_impl ${SOURCES})
