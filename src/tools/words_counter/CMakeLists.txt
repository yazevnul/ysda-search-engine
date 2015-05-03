project(words_counter)

set(SOURCES
    main.cpp
)

include_directories("${CMAKE_CURRENT_LIST_DIR}/../..")

add_executable(words_counter ${SOURCES})
target_link_libraries(words_counter
    ${CMAKE_THREAD_LIBS_INIT}
)
