include(FindPackageHandleStandardArgs)

#find_package(Python REQUIRED QUIET)

find_program(Sphinx_Build_EXECUTABLE NAMES sphinx-build)

if (Sphinx_Build_EXECUTABLE AND NOT Sphinx_Build_VERSION)
  execute_process(
    COMMAND ${Sphinx_Build_EXECUTABLE} --version
    OUTPUT_VARIABLE Sphinx_Build_VERSION_OUTPUT
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ENCODING UTF-8)
  if (Sphinx_Build_VERSION_OUTPUT)
    string(REGEX
      MATCH "[^0-9]+([0-9]+)[.]([0-9]+)?[.]?([0-9]+)?[.]?([0-9]+)?.*"
      Sphinx_Build_VERSION_CHECK
      ${Sphinx_Build_VERSION_OUTPUT})
    if (Sphinx_Build_VERSION_CHECK)
      string(JOIN "." Sphinx_Build_VERSION
        ${CMAKE_MATCH_1}
        ${CMAKE_MATCH_2}
        ${CMAKE_MATCH_3}
        ${CMAKE_MATCH_4})
      set(Sphinx_Build_VERSION "${Sphinx_Build_VERSION}" CACHE STRING "Sphinx Version" FORCE)
    endif()
  endif()
endif()

if (Sphinx_Build_EXECUTABLE)
  set(Sphinx_Build_FOUND YES)
endif()

find_package_handle_standard_args(Sphinx
  REQUIRED_VARS Sphinx_Build_EXECUTABLE
  VERSION_VAR Sphinx_Build_VERSION
  HANDLE_COMPONENTS)

mark_as_advanced(Sphinx_Build_EXECUTABLE Sphinx_Build_VERSION)

if (Sphinx_Build_FOUND AND NOT TARGET Sphinx::Build)
  add_executable(Sphinx::Build IMPORTED)
  set_property(TARGET Sphinx::Build PROPERTY IMPORTED_LOCATION ${Sphinx_Build_EXECUTABLE})
  set_property(TARGET Sphinx::Build PROPERTY VERSION ${Sphinx_Build_VERSION})
endif()
