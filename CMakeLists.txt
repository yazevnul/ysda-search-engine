cmake_minimum_required(VERSION 2.8)
project(ysda-search-engine)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(BUILD_STATIC_LIBS ON)
set(BUILD_SHARED_LIBS OFF)

### Compiler options

## Build type independent options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")

# Compiler dependent options
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c++98-compat-pedantic")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-padded")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-prototypes")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")

elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    message(SEND_ERROR "${CMAKE_CXX_COMPILER_ID} is not supported. WTF???")
else()
    message(SEND_ERROR "${CMAKE_CXX_COMPILER_ID} is not supported")
endif()


add_subdirectory(src)
