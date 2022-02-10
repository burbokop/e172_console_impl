cmake_minimum_required(VERSION 3.5)

project(console_impl LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(console_impl SHARED
    ${CMAKE_CURRENT_LIST_DIR}/src/consolerenderer.h
    ${CMAKE_CURRENT_LIST_DIR}/src/consolerenderer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/colorizer/colorizer.h
    ${CMAKE_CURRENT_LIST_DIR}/src/colorizer/colorizer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/surface.h
    ${CMAKE_CURRENT_LIST_DIR}/src/surface.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/pixelprimitives.h
    ${CMAKE_CURRENT_LIST_DIR}/src/pixelprimitives.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/png_reader.h
    ${CMAKE_CURRENT_LIST_DIR}/src/png_reader.cpp
    )

find_package(PNG REQUIRED)
target_link_libraries(console_impl
    e172
    ${PNG_LIBRARY}
    )

add_executable(console_impl_example
    ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/painter.h
    ${CMAKE_CURRENT_LIST_DIR}/src/painter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mp4_decoder.h
    ${CMAKE_CURRENT_LIST_DIR}/src/mp4_decoder.cpp
    )


target_link_libraries(console_impl_example
    console_impl
    e172
    SDL2
    avcodec
    avformat
    avfilter
    avdevice
    swresample
    swscale
    avutil
    )
