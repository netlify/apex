#ifndef APEX_CORE_MACROS_HPP
#define APEX_CORE_MACROS_HPP

/* This header is where we do a bunch of macro checking for platform related
 * preprocessor defines and a few headers. We try to be complete, regardless of
 * whether the platform is actually supported or not.
 *
 * That said, we *rarely* rely on these macros, except for exporting them as
 * constexpr usable traits
 */

#define APEX_COMPILER_UNKNOWN 0
#define APEX_SYSTEM_UNKNOWN 0
#define APEX_ENDIAN_UNKNOWN 0
#define APEX_ARCH_UNKNOWN 0

// AppleClang is considered a different compiler than Clang because it tends to
// be behind one way or another.
#define APEX_COMPILER_APPLECLANG 0
#define APEX_COMPILER_CLANG 0
#define APEX_COMPILER_MSVC 0
#define APEX_COMPILER_GCC 0

#define APEX_SYSTEM_WINDOWS 0
#define APEX_SYSTEM_FREEBSD 0
#define APEX_SYSTEM_OPENBSD 0
#define APEX_SYSTEM_NETBSD 0
#define APEX_SYSTEM_DRAGON 0
#define APEX_SYSTEM_MACOS 0
#define APEX_SYSTEM_LINUX 0

#define APEX_ARCH_AMD64 0
#define APEX_ARCH_ARM64 0

#define APEX_SIMD_SSE42 0
#define APEX_SIMD_SSE41 0
#define APEX_SIMD_SSSE3 0
#define APEX_SIMD_SSE3 0
#define APEX_SIMD_SSE2 0

#define APEX_SIMD_AVX512 0
#define APEX_SIMD_AVX2 0
#define APEX_SIMD_AVX 0

#define APEX_SIMD_NEON 0

#if defined(__clang__) and not defined(__apple_build_version__)
  #undef APEX_COMPILER_CLANG
  #define APEX_COMPILER_CLANG 1
#elif defined(__apple_build_version__)
  #undef APEX_COMPILER_APPLECLANG
  #define APEX_COMPILER_APPLECLANG 1
#elif defined(_MSC_VER)
  #undef APEX_COMPILER_MSVC
  #define APEX_COMPILER_MSVC 1
#elif defined(__GNUC__)
  #undef APEX_COMPILER_GCC
  #define APEX_COMPILER_GCC 1
#else
  #undef APEX_COMPILER_UNKNOWN
  #define APEX_COMPILER_UNKNOWN
#endif /* defined(__clang__) and not defined(__apple_build_version__) */

#if defined(_WIN32) or defined(_WIN64)
  #undef APEX_SYSTEM_WINDOWS
  #define APEX_SYSTEM_WINDOWS 1
#elif defined(__FreeBSD__)
  #undef APEX_SYSTEM_FREEBSD
  #define APEX_SYSTEM_FREEBSD 1
#elif defined(__OpenBSD__)
  #undef APEX_SYSTEM_OPENBSD
  #define APEX_SYSTEM_OPENBSD 1
#elif defined(__NetBSD__)
  #undef APEX_SYSTEM_NETBSD
  #define APEX_SYSTEM_NETBSD 1
#elif defined(__DragonFly__)
  #undef APEX_SYSTEM_DRAGON
  #define APEX_SYSTEM_DRAGON 1
#elif defined(__APPLE__)
  #undef APEX_SYSTEM_MACOS
  #define APEX_SYSTEM_MACOS 1
#elif defined(__linux__)
  #undef APEX_SYSTEM_LINUX
  #define APEX_SYSTEM_LINUX 1
#else
  #undef APEX_SYSTEM_UNKNOWN
  #define APEX_SYSTEM_UNKNOWN 1
#endif /* defined(_WIN32) or defined(_WIN64) */

#if defined(__amd64__) or defined(__amd64) or \
    defined(__x86_64__) or defined(__x86_64) or \
    defined(_M_AMD64) or defined(_M_X64)
  #undef APEX_ARCH_AMD64
  #define APEX_ARCH_AMD64
#elif defined(__aarch64__) or defined(__arm64) or defined(_M_ARM64)
  #undef APEX_ARCH_ARM64
  #define APEX_ARCH_ARM64 1
#else
  #undef APEX_ARCH_UNKNOWN
  #define APEX_ARCH_UNKNOWN 1
#endif /* defined(__amd64__) */

// TODO: Handle SIMD detection! Will be needed for some neat stuff we can
// do later 🙂

// TODO: Consider removing these. Just because they're unknown doesn't mean
// they won't work!
#if APEX_COMPILER_UNKNOWN
  #error "Unknown compiler detected"
#endif /* APEX_COMPILER_UNKNOWN */

#if APEX_SYSTEM_UNKNOWN
  #error "Unknown operating system detected"
#endif /* APEX_SYSTEM_UNKNOWN */

#if APEX_ARCH_UNKNOWN
  #error "Unknown architecture detected"
#endif /* APEX_ARCH_UNKNOWN */

#define APEX_IMPL_STRINGIFY(x) #x
#define APEX_STRINGIFY(x) APEX_IMPL_STRINGIFY(x)

#define APEX_CONCAT(x, y) APEX_PASTE(x, y)
#define APEX_PASTE(x, y) x##y

#define APEX_CHECK_API(name, version) (APEX_CONCAT(__cpp_lib_, name) >= APEX_CONCAT(version, L))
#define APEX_CHECK_CXX(name, version) (APEX_CONCAT(__cpp_, name) >= APEX_CONCAT(version, L))

#endif /* APEX_CORE_MACROS_HPP */
