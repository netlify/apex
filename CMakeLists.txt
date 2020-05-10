cmake_minimum_required(VERSION 3.16)
# TODO: Look into using IXM at some point once it's stable :)
project(apex
  VERSION 0.1.0
  LANGUAGES CXX)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

include(CheckCXXCompilerFlag)

include(CMakeDependentOption)
include(CMakePushCheckState)
include(GNUInstallDirs)
include(FetchContent)
include(CTest)

# Needs to handle a few more things I reckon :)
if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  option(APEX_BUILD_TESTS "Build unit tests" ${BUILD_TESTING})
endif()

check_cxx_compiler_flag(-Wstrict-aliasing CAN_WARN_STRICT_ALIASING)
check_cxx_compiler_flag(-Wuninitialized CAN_WARN_UNINITIALIZED)
check_cxx_compiler_flag(-Wuseless-cast CAN_WARN_USELESS_CAST)
check_cxx_compiler_flag(-Wcast-align CAN_WARN_CAST_ALIGN)
check_cxx_compiler_flag(-Wpedantic CAN_WARN_PEDANTIC)
check_cxx_compiler_flag(-Wextra CAN_WARN_EXTRA)
check_cxx_compiler_flag(-Wall CAN_WARN_DEFAULT)

check_cxx_compiler_flag(-mavx2 CAN_USE_MODE_AVX2)

cmake_push_check_state()
set(CMAKE_REQUIRED_LINK_OPTIONS -fsanitize=address)
check_cxx_compiler_flag(-fsanitize=address CAN_SANITIZE_ADDRESS)
cmake_pop_check_state()

set(--warn-strict-aliasing $<$<BOOL:${CAN_WARN_STRICT_ALIASING}>:-Wstrict-aliasing>)
set(--warn-uninitialized $<$<BOOL:${CAN_WARN_UNINITIALIZED}>:-Wuninitialized>)
set(--warn-useless-cast $<$<BOOL:${CAN_WARN_USELESS_CAST}>:-Wuseless-cast>)
set(--warn-cast-align $<$<BOOL:${CAN_WARN_CAST_ALIGN}>:-Wcast-align>)
set(--warn-pedantic $<$<BOOL:${CAN_WARN_PEDANTIC}>:-Wpedantic>)
set(--warn-default $<$<BOOL:${CAN_WARN_DEFAULT}>:-Wall>)
set(--warn-extra $<$<BOOL:${CAN_WARN_EXTRA}>:-Wextra>)

set(--sanitize-address $<$<BOOL:${CAN_SANITIZE_ADDRESS}>:-fsanitize=address>)
set(--use-mode-avx2 $<$<BOOL:${CAN_USE_MODE_AVX2}>:-mavx2>)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/*.cxx")

add_library(apex)
add_library(netlify::apex ALIAS apex)
target_compile_features(apex PRIVATE cxx_std_17)
target_compile_options(apex
  PUBLIC
    ${--use-mode-avx2}
  PRIVATE
    $<$<AND:$<CXX_COMPILER_ID:Clang>,$<PLATFORM_ID:Windows>>:-fansi-escape-codes>
    $<$<CXX_COMPILER_ID:Clang>:-fcolor-diagnostics>
    $<$<CXX_COMPILER_ID:GNU>:-fdiagnostics-color=always>
    $<$<CONFIG:Debug>:${--sanitize-address}>
    ${--warn-strict-aliasing}
    ${--warn-uninitalized}
    ${--warn-useless-cast}
    ${--warn-cast-align}
    ${--warn-pedantic}
    ${--warn-default}
    ${--warn-extra})
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
  target_compile_options(${target}
    PRIVATE
      $<$<AND:$<CXX_COMPILER_ID:Clang>,$<PLATFORM_ID:Windows>>:-fansi-escape-codes>
      $<$<CXX_COMPILER_ID:Clang>:-fcolor-diagnostics>
      $<$<CXX_COMPILER_ID:GNU>:-fdiagnostics-color=always>
    PUBLIC
      ${--sanitize-address}
      ${--warn-strict-aliasing}
      ${--warn-uninitalized}
      ${--warn-useless-cast}
      ${--warn-cast-align}
      ${--warn-pedantic}
      ${--warn-default}
      ${--warn-extra})
  target_link_options(${target}
    PUBLIC
      ${--sanitize-address})

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
