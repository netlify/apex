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
  option(APEX_BUILD_TESTS "Build unit tests" ${BUILD_TESTING})
endif()

file(GLOB sources CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/*.cxx")

add_library(apex)
add_library(netlify::apex ALIAS apex)
target_compile_features(apex PRIVATE cxx_std_17)
target_compile_options(apex
  PRIVATE
    $<$<AND:$<CXX_COMPILER_ID:Clang>,$<PLATFORM_ID:Windows>>:-fansi-escape-codes>
    $<$<CXX_COMPILER_ID:Clang>:-fcolor-diagnostics>
    $<$<CXX_COMPILER_ID:GNU>:-fdiagnostics-color=always>)
target_include_directories(apex
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_sources(apex PRIVATE ${sources})

install(TARGETS apex
  EXPORT netlify-apex
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT netlify-apex
  DESTINATION ${CMAKE_INSTALL_DATADIR}/cmake/netlify-apex
  NAMESPACE netlify::)
install(DIRECTORY include/ TYPE INCLUDE FILES_MATCHING PATTERN "*.hpp")

if (APEX_BUILD_TESTS)
endif()
