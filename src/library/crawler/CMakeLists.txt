project(crawler)

set(SOURCES
    simple_wikipedia_crawler.cpp
    url_extractor.cpp
)

include_directories("${CMAKE_CURRENT_LIST_DIR}/../..")

add_library(crawler ${SOURCES})
