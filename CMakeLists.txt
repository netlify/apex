cmake_minimum_required(VERSION 3.16)
include(FetchContent)
FetchContent_Declare(cmake GIT_REPOSITORY https://github.com/netlify/cmake)
FetchContent_MakeAvailable(cmake)
set(CMAKE_PROJECT_INCLUDE ${NETLIFY_PROJECT_PRELUDE})
# TODO: Look into using IXM at some point once it's stable :)
project(apex
  HOMEPAGE_URL "https://github.com/netlify/apex"
  VERSION 0.1.0
  LANGUAGES CXX C)

if (CMAKE_CXX_CLANG_TIDY)
  list(APPEND CMAKE_CXX_CLANG_TIDY --checks=readability-identifier-naming)
endif()

file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/*.cxx")

# TODO: Eventually replace this with a CMake wrapper project
FetchContent_Declare(sqlite3 URL "https://sqlite.org/2020/sqlite-amalgamation-3320100.zip")
if (NOT sqlite3_POPULATED)
  # notice that we do not call `add_subdirectory`, we simply fetch and then
  # use the subdirectory sources directly.
  FetchContent_Populate(sqlite3)
endif()

add_library(apex)
add_library(netlify::apex ALIAS apex)
target_compile_features(apex PUBLIC cxx_std_20)
target_compile_options(apex
  PUBLIC
    ${--use-mode-avx2}
  PRIVATE
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
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  PRIVATE
    $<BUILD_INTERFACE:${sqlite3_SOURCE_DIR}>)
target_sources(apex
  PRIVATE
    ${sqlite3_SOURCE_DIR}/sqlite3.c
    ${sources})

install(TARGETS apex
  EXPORT netlify-apex
  ARCHIVE COMPONENT Development
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT netlify-apex
  DESTINATION ${CMAKE_INSTALL_DATADIR}/cmake/netlify-apex
  COMPONENT Development
  NAMESPACE netlify::)
install(DIRECTORY include/
  TYPE INCLUDE
  COMPONENT Development
  FILES_MATCHING
    PATTERN "*.hpp")

if (APEX_BUILD_TESTS)
  target_link_libraries(netlify-apex-tests INTERFACE netlify::apex)
endif()

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  feature_summary(DEFAULT_DESCRIPTION WHAT ALL)
endif()
