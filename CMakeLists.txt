cmake_minimum_required(VERSION 3.16)

project(money_track_server)

include(debug.cmake)

if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/common/CMakeLists.txt")
    add_subdirectory(common)
endif()

if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/falcon_generic_server/CMakeLists.txt")
    add_subdirectory(falcon_generic_server)
endif()

set(SRC_FILES
    src/common/packet_manager.c
#     src/prog_feats/login.c
#     src/prog_feats/signup.c
    src/main.c
    src/cli.c
    src/client_session.c
)

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
    ./include
)

target_link_libraries(
    ${PROJECT_NAME}
    LINK_PUBLIC
    falcon_generic_server
    common_shared
    common_static
)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)