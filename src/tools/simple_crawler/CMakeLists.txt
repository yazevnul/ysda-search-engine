project(simple_crawler)

set(SOURCES
    main.cpp
)

include_directories("${CMAKE_CURRENT_LIST_DIR}/../..")

add_executable(simple_crawler ${SOURCES})
target_link_libraries(simple_crawler
    ${CMAKE_THREAD_LIBS_INIT}
    crawler
    download
    json
    mod_chooser
    g3logger
)
