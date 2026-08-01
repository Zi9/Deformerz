cmake_minimum_required(VERSION 3.5)
project(Deformers C CXX)

file(GLOB_RECURSE SRC src/*.c)
file(GLOB SRC_RLIMGUI vendor/rlImGui/*.cpp)
file(GLOB SRC_IMGUI vendor/cimgui/imgui/*.cpp)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE debug CACHE STRING "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c23 -Wall -Wextra -fPIC -fsanitize=address")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -fPIC -fsanitize=address")

add_subdirectory(vendor/raylib)
add_subdirectory(vendor/cimgui)

add_custom_command(
    OUTPUT src/Engine/Rendering/Shaders.h
    COMMAND python ../tools/shaderbuild.py ../src/Engine/Rendering/Shaders/ ../src/Engine/Rendering/
    DEPENDS ./src/Engine/Rendering/Shaders/Affine.fs ./src/Engine/Rendering/Shaders/Affine.vs
)

add_executable(Deformers ${SRC} ${SRC_RLIMGUI} src/Engine/Rendering/Shaders.h)

target_include_directories(Deformers PUBLIC src)
target_include_directories(Deformers PUBLIC vendor/rlImGui)

target_link_libraries(Deformers raylib cimgui m)
