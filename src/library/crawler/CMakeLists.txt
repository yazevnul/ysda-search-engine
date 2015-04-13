project(crawler)

add_subdirectory(simple_wikipedia_crawler_impl)

set(SOURCES
    simple_wikipedia_crawler.cpp
    url_extractor.cpp
)

include_directories("${CMAKE_CURRENT_LIST_DIR}/../..")

add_library(crawler ${SOURCES})
target_link_libraries(crawler
    simple_wikipedia_crawler_impl
)
