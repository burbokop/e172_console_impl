set(FFMPEG_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/ffmpeg_install_prefix)

include(ProcessorCount)
ProcessorCount(NPROC)

include(ExternalProject)

ExternalProject_Add(
  FFmpeg
  GIT_REPOSITORY https://github.com/FFmpeg/FFmpeg.git
  GIT_TAG n5.1.3
  INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/ffmpeg_install
  CONFIGURE_COMMAND "<SOURCE_DIR>/configure" "--prefix=${FFMPEG_INSTALL_PREFIX}"
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
target_include_directories(console_video_player
                           PRIVATE ${FFMPEG_INSTALL_PREFIX}/include)
target_link_directories(console_video_player PRIVATE
                        ${FFMPEG_INSTALL_PREFIX}/lib)

target_link_libraries(
  console_video_player
  e172
  e172_console_impl
  SDL2
  avdevice
  avfilter
  postproc
  avformat
  avcodec
  lzma
  dl
  Xfixes
  Xext
  X11
  asound
  SDL
  vorbisenc
  vorbis
  theoraenc
  theoradec
  ogg
  opencore-amrwb
  opencore-amrnb
  rt
  swresample
  swscale
  avutil
  m
  z
  faac
  mp3lame
  vpx
  x264
  xvidcore
  va
  va-drm
  va-x11
  bz2
  pthread)
