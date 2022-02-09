cmake_minimum_required(VERSION 3.5)

project(console_impl LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(console_impl SHARED
    consolerenderer.h
    consolerenderer.cpp
    color.h
    color.cpp
    surface.h
    surface.cpp
    pixelprimitives.h
    pixelprimitives.cpp
    png_reader.h
    png_reader.cpp
    )

find_package(PNG REQUIRED)
target_link_libraries(console_impl
    e172
    ${PNG_LIBRARY}
    )

add_executable(console_impl_example
    main.cpp
    painter.h
    painter.cpp
    )


target_link_libraries(console_impl_example
    console_impl
    e172
    SDL2
    )
