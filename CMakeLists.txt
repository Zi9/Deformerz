cmake_minimum_required(VERSION 3.5)
project(Deformerz C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE debug CACHE STRING "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
set(IMGUI_STATIC "yes" CACHE STRING "Build as a static library" FORCE)

if(WIN32)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /std:c23 /Wall /Zi")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall /Zi")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c23 -Wall -Wextra -fPIC -fsanitize=address")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -fPIC -fsanitize=address")
endif()

# 1st party sources
file(GLOB_RECURSE SRC_DEFORMERZ_ENGINE src/Engine/*.c)
file(GLOB_RECURSE SRC_DEFORMERZ_CONTEXTS src/Contexts/*.c)
file(GLOB_RECURSE SRC_LIBTEREP src/LibTerep/*.c)
file(GLOB SRC_TOOLS src/Tools/*.c)

# 3rd party sources
file(GLOB SRC_RLIMGUI vendor/rlImGui/*.cpp)
file(GLOB SRC_IMGUI vendor/cimgui/imgui/*.cpp)
add_subdirectory(vendor/raylib)
add_subdirectory(vendor/cimgui)

add_library(LibTerep STATIC ${SRC_LIBTEREP})
target_include_directories(LibTerep PUBLIC src)
if(NOT WIN32)
    target_link_libraries(LibTerep PUBLIC m)
endif()

add_custom_command(
    OUTPUT src/Engine/Shaders.h
    COMMAND python ../scripts/shaderbuild.py ../src/Engine/Shaders/ ../src/Engine/Shaders.h
    DEPENDS ./src/Engine/Shaders/Affine.fs ./src/Engine/Shaders/Affine.vs
)

if(WIN32)
    add_executable(Deformerz
        ${SRC_DEFORMERZ_ENGINE}
        ${SRC_DEFORMERZ_CONTEXTS}
        ${SRC_RLIMGUI} ${SRC_IMGUI}
        src/Engine/Shaders.h
    )
    target_include_directories(Deformerz PUBLIC vendor/cimgui/imgui)
    target_link_libraries(Deformerz LibTerep raylib cimgui)
else()
    add_executable(Deformerz
        ${SRC_DEFORMERZ_ENGINE}
        ${SRC_DEFORMERZ_CONTEXTS}
        ${SRC_RLIMGUI} ${SRC_IMGUI}
        src/Engine/Shaders.h
    )
    target_link_libraries(Deformerz LibTerep raylib cimgui m)
endif()

target_include_directories(Deformerz PUBLIC src)
target_include_directories(Deformerz PUBLIC vendor/rlImGui)


foreach(TOOL_SRC ${SRC_TOOLS})
    get_filename_component(TOOL_NAME ${TOOL_SRC} NAME_WE)
    add_executable(${TOOL_NAME} ${TOOL_SRC})
    target_link_libraries(${TOOL_NAME} PRIVATE LibTerep)
    target_include_directories(${TOOL_NAME} PRIVATE src)
endforeach()
