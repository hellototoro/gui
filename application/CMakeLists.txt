cmake_minimum_required(VERSION 3.12.4)

project(application)

file(GLOB_RECURSE SOURCES ./ui/*.c)

add_library(application STATIC ${SOURCES} windows.c init.c)
