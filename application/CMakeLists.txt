cmake_minimum_required(VERSION 3.12.4)

project(application)

include_directories(
    ${PROJECT_SOURCE_DIR}/core/
)

file(GLOB_RECURSE SOURCES ./*.c
                          ./*.cpp)

add_library(application STATIC ${SOURCES})
