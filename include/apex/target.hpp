#ifndef APEX_TARGET_HPP
#define APEX_TARGET_HPP

// FIXME: `linux` is a preprocessor define when extensions are enabled.
// That means this header can't be included, which defeats the dang purpose!

#include <apex/macros.hpp>

namespace apex {
inline namespace v1 {
namespace target {

enum class compiler {
  unknown,
  apple,
  clang,
  msvc,
  gcc,
};

enum class system {
  unknown,
  windows,
  freebsd,
  openbsd,
  netbsd,
  dragon,
  macos,
  linux,
};

enum class arch {
  unknown,
  amd64,
  arm64,
};

enum class simd {
  unknown,
  avx512,
  avx2,
  avx,
  sse42,
  sse41,
  ssse3,
  sse3,
  sse2,
  neon,
};

struct current final {
  static constexpr inline auto compiler = [] () constexpr {
    if constexpr (APEX_COMPILER_APPLECLANG) { return compiler::apple; }
    else if constexpr (APEX_COMPILER_CLANG) { return compiler::clang; }
    else if constexpr (APEX_COMPILER_MSVC) { return compiler::msvc; }
    else if constexpr (APEX_COMPILER_GCC) { return compiler::gcc; }
    else { return compiler::unknown; }
  }();

  static constexpr inline auto system = [] () constexpr {
    if constexpr (APEX_SYSTEM_WINDOWS) { return system::windows; }
    else if constexpr (APEX_SYSTEM_FREEBSD) { return system::freebsd; }
    else if constexpr (APEX_SYSTEM_OPENBSD) { return system::openbsd; }
    else if constexpr (APEX_SYSTEM_NETBSD) { return system::netbsd; }
    else if constexpr (APEX_SYSTEM_DRAGON) { return system::dragon; }
    else if constexpr (APEX_SYSTEM_MACOS) { return system::macos; }
    else if constexpr (APEX_SYSTEM_LINUX) { return system::linux; }
    else { return system::unknown; }
  }();

  static constexpr inline auto arch = [] () constexpr {
    if constexpr (APEX_ARCH_AMD64) { return system::amd64; }
    else if constexpr (APEX_ARCH_ARM64) { return system::arm64; }
    else { return system::unknown; }
  }();

  static constexpr inline auto simd = [] () constexpr {
    if constexpr (APEX_SIMD_AVX512) { return simd::avx512; }
    else if constexpr (APEX_SIMD_AVX2) { return simd::avx2; }
    else if constexpr (APEX_SIMD_AVX) { return simd::avx; }
    else if constexpr (APEX_SIMD_SSE42) { return simd::sse42; }
    else if constexpr (APEX_SIMD_SSE41) { return simd::sse41; }
    else if constexpr (APEX_SIMD_SSSE3) { return simd::ssse3; }
    else if constexpr (APEX_SIMD_SSE3) { return simd::sse3; }
    else if constexpr (APEX_SIMD_SSE2) { return simd::sse2; }
    else if constexpr (APEX_SIMD_NEON) { return simd::neon; }
    else { return simd::unknown; }
  }();
};

}}} /* namespace apex::v1::target */

#endif /* APEX_TARGET_HPP */
