#ifndef APEX_CORE_SOURCE_HPP
#define APEX_CORE_SOURCE_HPP

#if __has_include(<source_location>)
  #include <source_location>
#endif /* __has_include(<source_location>) */

#include <apex/core/macros.hpp>

namespace apex {

#if APEX_CHECK_API(source_location, 201907)
using ::std::source_location;
#else
struct source_location {

  constexpr source_location () noexcept = default;

  constexpr auto function_name () const noexcept { return this->func; }
  constexpr auto file_name () const noexcept { return this->file; }
  constexpr auto column () const noexcept { return this->col; }
  constexpr auto line () const noexcept { return this->ln; }

  static constexpr source_location current (
    char const* func = __builtin_FUNCTION(),
    char const* file = __builtin_FILE(),

    unsigned int col = __builtin_COLUMN(),
    unsigned int ln = __builtin_LINE()) noexcept {
    source_location loc;
    loc.set(func, file, col, ln);
    return loc;
  }

private:
  constexpr void set (char const* func, char const* file, unsigned int col, unsigned int ln) noexcept {
    this->func = func;
    this->file = file;
    this->col = col;
    this->ln = ln;
  }

  char const* func = nullptr;
  char const* file = nullptr;
  unsigned int col = 0;
  unsigned int ln = 0;
};
#endif /* APEX_CHECK_API(source_location, 201907) */
} /* namespace apex */

#endif /* APEX_CORE_SOURCE_HPP */
