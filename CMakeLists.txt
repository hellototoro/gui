cmake_minimum_required(VERSION 3.10)
project(projector_gui)
set(CMAKE_C_STANDARD 11)#C11
set(CMAKE_CXX_STANDARD 17)#C17
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    ${PROJECT_SOURCE_DIR}/application/
    ${PROJECT_SOURCE_DIR}/boost_1_79_0/
    ${PROJECT_SOURCE_DIR}/lv_host_sim/
    ${PROJECT_SOURCE_DIR}/lv_i18n/
    ${PROJECT_SOURCE_DIR}
)

#file(GLOB_RECURSE INCLUDES "./*.h" )

add_subdirectory(application)
add_subdirectory(lv_i18n)
add_subdirectory(lv_host_sim)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(CMAKE_CXX_FLAGS "-O3")

add_executable(lvgl_app main.c ${SOURCES} ${INCLUDES})
target_link_libraries(lvgl_app PRIVATE lvgl_lib application lv_i18n)
add_custom_target (run COMMAND ${EXECUTABLE_OUTPUT_PATH}/lvgl_app)
