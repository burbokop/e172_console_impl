cmake_minimum_required(VERSION 3.26.3)

project(
  e172_console_impl
  LANGUAGES CXX
  VERSION 0.7.0
  DESCRIPTION "Console implementation of e172"
  HOMEPAGE_URL "https://github.com/burbokop/e172_console_impl")

option(ENABLE_FIND_E172_PACKAGE
       "Find e172 package (searches for link and include directories if OFF)"
       ON)
option(ENABLE_EXAMPLES "Enable examples" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

include(GNUInstallDirs)

add_library(${PROJECT_NAME} SHARED)

set(INSTALL_NAMESPACE e172)
set(INSTALL_IMPL_NAMESPACE console)

set(INSTALLDIR
    "${CMAKE_INSTALL_INCLUDEDIR}/${INSTALL_NAMESPACE}/impl/${INSTALL_IMPL_NAMESPACE}"
)

target_sources(
  ${PROJECT_NAME}
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src/renderer.h>
         $<INSTALL_INTERFACE:${INSTALLDIR}/renderer.h>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src/graphicsprovider.h>
         $<INSTALL_INTERFACE:${INSTALLDIR}/graphicsprovider.h>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src/eventprovider.h>
         $<INSTALL_INTERFACE:${INSTALLDIR}/eventprovider.h>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src/colorizer/colorizer.h>
         $<INSTALL_INTERFACE:${INSTALLDIR}/colorizer/colorizer.h>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src/surface.h>
         $<INSTALL_INTERFACE:${INSTALLDIR}/surface.h>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src/pixelprimitives.h>
         $<INSTALL_INTERFACE:${INSTALLDIR}/pixelprimitives.h>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src/png_reader.h>
         $<INSTALL_INTERFACE:${INSTALLDIR}/png_reader.h>
  PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src/renderer.cpp
          ${CMAKE_CURRENT_LIST_DIR}/src/graphicsprovider.cpp
          ${CMAKE_CURRENT_LIST_DIR}/src/eventprovider.cpp
          ${CMAKE_CURRENT_LIST_DIR}/src/colorizer/colorizer.cpp
          ${CMAKE_CURRENT_LIST_DIR}/src/surface.cpp
          ${CMAKE_CURRENT_LIST_DIR}/src/pixelprimitives.cpp
          ${CMAKE_CURRENT_LIST_DIR}/src/png_reader.cpp)

find_package(PNG REQUIRED)

if(ENABLE_FIND_E172_PACKAGE)
  find_package(e172 REQUIRED)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE ${PNG_LIBRARY})

if(ENABLE_FIND_E172_PACKAGE)
  target_link_libraries(${PROJECT_NAME} PRIVATE e172::e172)
else()
  target_link_libraries(${PROJECT_NAME} PRIVATE e172)
endif()

if((CMAKE_CXX_COMPILER_ID STREQUAL "Clang") OR (CMAKE_CXX_COMPILER_ID STREQUAL
                                                "GNU"))
  target_compile_options(${PROJECT_NAME} PRIVATE -Werror -Wall -Wextra)
endif()

if(ENABLE_EXAMPLES)
  add_subdirectory(examples)
endif()

install(
  TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}_targets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(
  DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/src/"
  DESTINATION
    "${CMAKE_INSTALL_INCLUDEDIR}/${INSTALL_NAMESPACE}/impl/${INSTALL_IMPL_NAMESPACE}"
  FILES_MATCHING
  PATTERN "*.h")

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${PROJECT_NAME}ConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMinorVersion)

configure_package_config_file(
  "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

install(
  EXPORT ${PROJECT_NAME}_targets
  FILE ${PROJECT_NAME}Targets.cmake
  NAMESPACE ${INSTALL_NAMESPACE}::
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
              "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
