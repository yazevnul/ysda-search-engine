project(json)


set(SOURCES
    ../../third_party/json11/json11.cpp
)

add_library(json ${SOURCES})
