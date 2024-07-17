cmake_minimum_required(VERSION 3.13.0)
project(common VERSION 1.0.0 DESCRIPTION "Common Calc Libraries for CSD")

set(SRC_FILES
    src/print_utilities.c
)

add_library(${PROJECT_NAME}_shared SHARED ${SRC_FILES})

target_include_directories(
    ${PROJECT_NAME}_shared
    PUBLIC
    ./include
)

add_library(${PROJECT_NAME}_static STATIC ${SRC_FILES})

target_include_directories(
    ${PROJECT_NAME}_static
    PUBLIC
    ./include
)