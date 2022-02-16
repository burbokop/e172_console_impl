cmake_minimum_required(VERSION 3.5)

project(console_impl LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(console_impl.cmake)

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
    ncurses
    avcodec
    avformat
    avfilter
    avdevice
    swresample
    swscale
    avutil
    )
