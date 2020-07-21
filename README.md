# Overview

Apex is Netlify's Core C++ library. In addition to providing shims for upcoming
C++ standards library features, it also provides several modules for types that
can't be standardized. Below is a non-exhaustive list of features, as well as
basic instructions for getting started building. Unlike many C++ libraries,
Apex is compiled and only used as a static library.

Apex is currently available under the Apache 2.0 License.

# Building

Apex currently targets C++20 and Linux. We only test and use it on Ubuntu.
Additionally, we stay up to date with a recent version of CMake. The current
mininimum version is CMake 3.16. We also only run the build with [ninja][1].
The CMake file is  setup in such a way that Apex can also be used via
`add_subdirectory`, making it possible to acquire via `FetchContent` during a
build.

## Configuring

To configure Apex, run the following command

```bash
cmake -Bbuild -S. -GNinja -DCMAKE_BUILD_TYPE=Release
```

Additional options can be set via `-D<OPTION>=<VALUE>`. These are documented
elsewhere.

## Compiling

Run the following command to build Apex

```bash
cmake --build build
```

## Dependencies

Apex will pull its dependencies in via [FetchContent][2]. The CMake
documentation has more information on hooking into or changing dependencies.

[1]: https://ninja-build.org
[2]: https://cmake.org/cmake/help/latest/module/FetchContent.html
