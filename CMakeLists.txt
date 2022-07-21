cmake_minimum_required(VERSION 3.10)
project(gui_lvgl)
set(CMAKE_C_STANDARD 11)#C11
set(CMAKE_CXX_STANDARD 17)#C17
set(CMAKE_CXX_STANDARD_REQUIRED ON)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})

file(GLOB_RECURSE INCLUDES "./*.h" )

add_definitions(-DHOST_GCC)

add_subdirectory(lvgl)
add_subdirectory(lv_drivers)
add_subdirectory(application)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(CMAKE_CXX_FLAGS "-O3")

find_package(SDL2 REQUIRED SDL2)
include_directories(${SDL2_INCLUDE_DIRS})
add_executable(lvgl_app main.c lv_host_sim/mouse_cursor_icon.c lv_host_sim/sdl_init.c ${SOURCES} ${INCLUDES})
add_compile_definitions(LV_CONF_INCLUDE_SIMPLE)
target_link_libraries(lvgl_app PRIVATE lvgl lvgl::examples lvgl::demos lvgl::drivers ${SDL2_LIBRARIES} application avformat avcodec avutil swscale m z pthread)
add_custom_target (run COMMAND ${EXECUTABLE_OUTPUT_PATH}/lvgl_app)
