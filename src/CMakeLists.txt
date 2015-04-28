project(search-engine)

find_package(Protobuf REQUIRED)
if(NOT PROTOBUF_FOUND)
    message(SEND_ERROR "Failed to find protobuf.")
else()
    include_directories(${PROTOBUF_INCLUDE_DIRS})
endif()

find_package(Threads REQUIRED)

add_subdirectory(library)
add_subdirectory(tools)
add_subdirectory(third_party)
