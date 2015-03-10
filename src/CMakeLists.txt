project(ysda)

find_package(Protobuf REQUIRED)
if(NOT PROTOBUF_FOUND)
    message(SEND_ERROR "Failed to find protobuf.")
else()
    include_directories(${PROTOBUF_INCLUDE_DIRS})
endif()

find_package(Threads REQUIRED)

add_subdirectory(library/accuarate_accumulate)
add_subdirectory(library/index)
add_subdirectory(library/protobuf_helpers)
add_subdirectory(library/timer)

add_subdirectory(tools/dictionary_parser)
add_subdirectory(tools/index_parser)
add_subdirectory(tools/ranker)
