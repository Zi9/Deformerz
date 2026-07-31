cmake_minimum_required(VERSION 3.5)
project(Deformers C CXX)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c23 -Wall -Wextra -fPIC -fsanitize=address")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE debug CACHE STRING "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

include_directories(src)

file(GLOB_RECURSE SRC src/*.c)
file(GLOB SRC_RLIMGUI vendor/rlImGui/*.cpp)
file(GLOB SRC_IMGUI vendor/cimgui/imgui/*.cpp)

add_custom_command(
    OUTPUT src/core/t_shaders.h
    COMMAND python ../tools/shaderbuild.py ../src/core/shaders/ ../src/core/
    DEPENDS ./src/core/shaders/affine.fs ./src/core/shaders/affine.vs
)

add_subdirectory(vendor/raylib)
add_subdirectory(vendor/cimgui)

add_executable(Deformers ${SRC} src/core/t_shaders.h)

target_include_directories(Deformers PUBLIC vendor/rlImGui)

target_link_libraries(Deformers raylib m)
