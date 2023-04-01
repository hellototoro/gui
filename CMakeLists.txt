cmake_minimum_required(VERSION 3.10)
project(projector_gui)
set(CMAKE_C_STANDARD 11)#C11
set(CMAKE_CXX_STANDARD 17)#C17
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    ${PROJECT_SOURCE_DIR}/application/
    ${PROJECT_SOURCE_DIR}/boost_1_79_0/
    ${PROJECT_SOURCE_DIR}/lvgl/
    ${PROJECT_SOURCE_DIR}/lv_drivers/
    ${PROJECT_SOURCE_DIR}/lv_i18n/
    ${PROJECT_SOURCE_DIR}/hlibc/src/
    ${PROJECT_SOURCE_DIR}
)

find_package(SDL2 REQUIRED SDL2)
include_directories(
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_INCLUDE_DIRS}/../
)

add_subdirectory(application)
add_subdirectory(lvgl)
add_subdirectory(lv_drivers)
add_subdirectory(lv_i18n)
add_subdirectory(hlibc/src)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(CMAKE_CXX_FLAGS "-O0 -g")

add_executable(lvgl_app main.cpp mouse_cursor_icon.c sdl_init.c ${SOURCES} ${INCLUDES})
target_link_libraries(lvgl_app PRIVATE lvgl lvgl::examples lvgl::demos lvgl::drivers ${SDL2_LIBRARIES} m z pthread application lv_i18n hlibc)
