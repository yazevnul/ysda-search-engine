project(simple_crawler_impl)

include_directories("${CMAKE_CURRENT_LIST_DIR}/../../..")

set(SOURCES
    coordinator.cpp
    queue.cpp
    url_to_id.cpp
    urls.cpp
)

add_library(simple_crawler_impl ${SOURCES})
target_link_libraries(simple_crawler_impl
    crawler_protos
)
