cmake_minimum_required(VERSION 3.16)

project(money_track_server)

if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/common/CMakeLists.txt")
    add_subdirectory(common)
endif()

set(SRC_FILES
    src/main.c
    src/cli.c
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