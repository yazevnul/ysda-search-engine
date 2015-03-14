project(crawler)

set(SOURCES
    main.cpp
)

include_directories("${CMAKE_CURRENT_LIST_DIR}/../..")

add_executable(crawler ${SOURCES})
target_link_libraries(crawler
    ${CMAKE_THREAD_LIBS_INIT}
    download
)
