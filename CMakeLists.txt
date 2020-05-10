cmake_minimum_required(VERSION 3.16)
# TODO: Look into using IXM at some point once it's stable :)
project(apex
  VERSION 0.1.0
  LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

include(CMakeDependentOption)
include(GNUInstallDirs)
include(FetchContent)
include(CTest)

# Needs to handle a few more things I reckon :)
if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  option(APEX_BUILD_TESTS "Build unit tests" ${BUILD_TESTING})
endif()

file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/*.cxx")

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
  set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
  set(CATCH_BUILD_TESTING OFF)
  set(CATCH_ENABLE_WERROR OFF)
  set(CATCH_INSTALL_HELPERS OFF)
  set(CATCH_INSTALL_DOCS OFF)
  FetchContent_Declare(catch
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_SHALLOW ON
    GIT_TAG v2.12.1)
  FetchContent_MakeAvailable(catch)
  file(GENERATE
    OUTPUT "${PROJECT_BINARY_DIR}/tests/catch.cxx"
    CONTENT [[
      #define CATCH_CONFIG_MAIN
      #include <catch2/catch.hpp>
    ]])
  set(target netlify-apex-tests)
  add_library(${target})
  add_library(netlify::apex::tests ALIAS ${target})
  target_compile_options(${target}
    PUBLIC
      $<$<AND:$<CXX_COMPILER_ID:Clang>,$<PLATFORM_ID:Windows>>:-fansi-escape-codes>
      $<$<CXX_COMPILER_ID:Clang>:-fcolor-diagnostics>
      $<$<CXX_COMPILER_ID:GNU>:-fdiagnostics-color=always>)
  target_compile_features(${target} PUBLIC cxx_std_17)
  target_link_libraries(${target} PUBLIC netlify::apex Catch2::Catch2)
  target_sources(${target} PRIVATE "${PROJECT_BINARY_DIR}/tests/catch.cxx")
  target_precompile_headers(${target} INTERFACE <catch2/catch.hpp>)

  file(GLOB_RECURSE sources
    RELATIVE "${PROJECT_SOURCE_DIR}/tests"
    CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/tests/*.cxx")
  foreach (source IN LISTS sources)
    get_filename_component(module "${source}" DIRECTORY)
    get_filename_component(name "${source}" NAME_WLE)
    set(target apex-test-${module}-${name})
    add_executable(${target})
    add_test(NAME test::apex::${module}::${name} COMMAND ${target})
    target_sources(${target} PRIVATE "${PROJECT_SOURCE_DIR}/tests/${source}")
    target_link_libraries(${target} PRIVATE netlify::apex::tests)
  endforeach()
endif()
