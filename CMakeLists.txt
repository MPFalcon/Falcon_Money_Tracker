cmake_minimum_required(VERSION 3.16)

project(money_track_server)

include(debug.cmake)

if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/common/CMakeLists.txt")
    add_subdirectory(common)
endif()

set(SRC_FILES
    src/common/entities.c
    src/prog_feats/login.c
    src/prog_feats/signup.c
    src/main.c
    src/cli.c
    src/client_session.c
    src/server_interface.c
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
    common_shared
    common_static
)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)