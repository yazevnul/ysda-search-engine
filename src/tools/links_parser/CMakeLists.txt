project(links_parser)

set(SOURCES
    main.cpp
)

include_directories("${CMAKE_CURRENT_LIST_DIR}/../..")

add_executable(links_parser ${SOURCES})
target_link_libraries(links_parser
    ${CMAKE_THREAD_LIBS_INIT}
    crawler
    download
)
