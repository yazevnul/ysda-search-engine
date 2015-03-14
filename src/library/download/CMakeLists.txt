project(download)

set(SOURCES
    wget.cpp
)

add_library(download ${SOURCES} ${HEADERS})
