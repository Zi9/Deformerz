cmake_minimum_required(VERSION 3.5)
project(Deformerz C CXX)

file(GLOB_RECURSE SRC src/*.c)
file(GLOB SRC_RLIMGUI vendor/rlImGui/*.cpp)
file(GLOB SRC_IMGUI vendor/cimgui/imgui/*.cpp)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE debug CACHE STRING "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

if(WIN32)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /std:c23 /Wall /Zi")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall /Zi")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c23 -Wall -Wextra -fPIC -fsanitize=address")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -fPIC -fsanitize=address")
endif()

add_subdirectory(vendor/raylib)

if(WIN32)
    set(IMGUI_STATIC "yes" CACHE STRING "Build as a static library" FORCE)
endif()
add_subdirectory(vendor/cimgui)

add_custom_command(
    OUTPUT src/Engine/Shaders.h
    COMMAND python ../tools/shaderbuild.py ../src/Engine/Shaders/ ../src/Engine/Shaders.h
    DEPENDS ./src/Engine/Shaders/Affine.fs ./src/Engine/Shaders/Affine.vs
)

if(WIN32)
    add_executable(Deformerz ${SRC} ${SRC_RLIMGUI} ${SRC_IMGUI} src/Engine/Shaders.h)
    target_include_directories(Deformerz PUBLIC vendor/cimgui/imgui)
else()
    add_executable(Deformerz ${SRC} ${SRC_RLIMGUI} src/Engine/Shaders.h)
endif()

target_include_directories(Deformerz PUBLIC src)
target_include_directories(Deformerz PUBLIC vendor/rlImGui)

if(WIN32)
    target_link_libraries(Deformerz raylib cimgui)
else()
    target_link_libraries(Deformerz raylib cimgui m)
endif()
