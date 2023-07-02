cmake_minimum_required(VERSION 3.26.3)

project(e172_console_impl LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_EXAMPLES "Enable examples" ON)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(e172_console_impl SHARED
    ${CMAKE_CURRENT_LIST_DIR}/src/renderer.h
    ${CMAKE_CURRENT_LIST_DIR}/src/renderer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/graphicsprovider.h
    ${CMAKE_CURRENT_LIST_DIR}/src/graphicsprovider.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/colorizer/colorizer.h
    ${CMAKE_CURRENT_LIST_DIR}/src/colorizer/colorizer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/surface.h
    ${CMAKE_CURRENT_LIST_DIR}/src/surface.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/pixelprimitives.h
    ${CMAKE_CURRENT_LIST_DIR}/src/pixelprimitives.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/png_reader.h
    ${CMAKE_CURRENT_LIST_DIR}/src/png_reader.cpp)

find_package(PNG REQUIRED)
target_link_libraries(e172_console_impl
    e172
    ${PNG_LIBRARY})

if(ENABLE_EXAMPLES)
    add_subdirectory(examples)
endif()
