project(crawler)

add_subdirectory(protos)
add_subdirectory(simple_crawler_impl)

set(SOURCES
    simple_crawler.cpp
    url_extractor.cpp
)

include_directories("${CMAKE_CURRENT_LIST_DIR}/../..")

add_library(crawler ${SOURCES})
target_link_libraries(crawler
    logger
    protobuf_helpers
    simple_crawler_impl
)
