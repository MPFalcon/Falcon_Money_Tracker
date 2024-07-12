cmake_minimum_required(VERSION 3.16)

#        Thread Pool Library
# --------------------------------

project(threadpool
        LANGUAGES "C")

set(SRC_POOL_FILES
    threadpool/src/threadpool.c
    threadpool/src/linked_list.c
    threadpool/src/signals.c
)

add_library(threadpool STATIC ${SRC_POOL_FILES})

target_include_directories(
    threadpool
    PUBLIC
    threadpool/include
)

target_link_libraries(
    threadpool
    LINK_PUBLIC
    common_static
)

#      Generic Server Library
# --------------------------------

project(falcon_generic_server
        LANGUAGES "C")

set(SRC_POOL_FILES
    src/net_io_stream.c
    src/manage_fds.c
    src/server_interface.c
)

add_library(falcon_generic_server STATIC ${SRC_POOL_FILES})

target_include_directories(
    falcon_generic_server
    PUBLIC
    ./include
)

target_link_libraries(
    falcon_generic_server
    LINK_PUBLIC
    threadpool
    common_static
)