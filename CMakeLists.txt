cmake_minimum_required(VERSION 3.16)
# TODO: Look into using IXM at some point once it's stable :)
project(apex
  VERSION 0.1.0
  LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

include(CMakeDependentOption)
include(GNUInstallDirs)
include(CTest)

# Needs to handle a few more things I reckon :)
if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  option(BUILD_TESTS "Build unit tests" ${BUILD_TESTING})
endif()

# This *will* change to a non-INTERFACE library at
# some point.
add_library(apex INTERFACE)
add_library(netlify::apex ALIAS apex)
target_include_directories(apex
  INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

install(TARGETS apex
  EXPORT netlify-apex
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT netlify-apex
  DESTINATION ${CMAKE_INSTALL_DATADIR}/cmake/netlify-apex
  NAMESPACE netlify::)
install(DIRECTORY include/ TYPE INCLUDE FILES_MATCHING PATTERN "*.hpp")

if (BUILD_TESTS)
endif()
