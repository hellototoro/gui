cmake_minimum_required(VERSION 3.12.4)

project(application)

file(GLOB_RECURSE SOURCES ./ui/*.c
                          ./ui/*.cpp
                          ./setting/*.c
                          ./setting/*.cpp)

add_library(application STATIC ${SOURCES} windows.c SystemInit.cpp ConfigParam.cpp NetWorkApi.c)
