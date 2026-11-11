set(FFMPEG_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/ffmpeg_install_prefix)

include(ProcessorCount)
ProcessorCount(NPROC)

find_package(BZip2 REQUIRED)

include(ExternalProject)

ExternalProject_Add(
    FFmpeg
    GIT_REPOSITORY https://github.com/FFmpeg/FFmpeg.git
    GIT_TAG n6.1.2
    INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/ffmpeg_install
    CONFIGURE_COMMAND "<SOURCE_DIR>/configure" "--prefix=${FFMPEG_INSTALL_PREFIX}" "--disable-doc"
    BUILD_COMMAND "make" "-j" "${NPROC}"
    INSTALL_COMMAND "make" "install")

add_executable(
    console_video_player
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/painter.h
    ${CMAKE_CURRENT_LIST_DIR}/painter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/mp4_decoder.h
    ${CMAKE_CURRENT_LIST_DIR}/mp4_decoder.cpp
    ${CMAKE_CURRENT_LIST_DIR}/videoplayer.h
    ${CMAKE_CURRENT_LIST_DIR}/videoplayer.cpp)

add_dependencies(console_video_player FFmpeg)
target_include_directories(console_video_player PRIVATE ${FFMPEG_INSTALL_PREFIX}/include)
target_link_directories(console_video_player PRIVATE ${FFMPEG_INSTALL_PREFIX}/lib)

if (ENABLE_FIND_E172_PACKAGE)
    target_link_libraries(${PROJECT_NAME} PRIVATE e172::e172)
else ()
    target_link_libraries(${PROJECT_NAME} PRIVATE e172)
endif ()

target_link_libraries(
    console_video_player
    PRIVATE # UNCOMMENT IF U ENCOUNTER RUNTIME ERRORS
            #
            # theoraenc theoradec opencore-amrwb opencore-amrnb faac mp3lame vpx x264 xvidcore va
            # va-drm va-x11 postproc vorbisenc vorbis ogg
            e172_console_impl
            SDL2
            avdevice
            avfilter
            avformat
            avcodec
            lzma
            dl
            Xfixes
            Xext
            X11
            asound
            rt
            swresample
            swscale
            avutil
            m
            z
            pthread
            BZip2::BZip2)
