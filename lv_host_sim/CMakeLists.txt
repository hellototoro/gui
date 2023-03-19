cmake_minimum_required(VERSION 3.12.4)

project(lvgl_lib)

find_package(SDL2 REQUIRED SDL2)
include_directories(
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_INCLUDE_DIRS}/../
)

add_subdirectory(lvgl)
add_subdirectory(lv_drivers)

add_library(${PROJECT_NAME} STATIC mouse_cursor_icon.c sdl_init.c)
#target_link_libraries(${PROJECT_NAME} lvgl lvgl::examples lvgl::demos lvgl::drivers ${SDL2_LIBRARIES} avformat avcodec avutil swscale m z pthread)
target_link_libraries(${PROJECT_NAME} lvgl lvgl::examples lvgl::demos lvgl::drivers ${SDL2_LIBRARIES} m z pthread)
