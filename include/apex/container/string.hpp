#ifndef APEX_CONTAINER_STRING_HPP
#define APEX_CONTAINER_STRING_HPP

#include <apex/core/macros.hpp>
#include <string>

#include <cstring>
#include <cctype>

// Contains string utilities, a zstring_view, and some additional char traits
namespace apex::container {

struct case_insensitive_traits : std::char_traits<char> {

  static bool eq (char c1, char c2) { return convert(c1) == convert(c2); }
  static bool lt (char c1, char c2) { return convert(c1) < convert(c2); }

  static int compare (char const* s1, char const* s2, size_t n) {
    #if APEX_SYSTEM_WINDOWS
      return _strnicmp(s1, s2, n);
    #else
      return strncasecmp(s1, s2, n);
    #endif /* APEX_SYSTEM_WINDOWS */
  }

  static auto find (char const*, size_t n, char const& a) noexcept {
    auto begin = s;
    auto end = s + n;
    auto iter = std::find_if(begin, end, [a = convert(a)] (auto c) noexcept {
      return convert(c) == a;
    });
    return iter != end ? iter : nullptr;
  }

private:
  auto convert (char ch) noexcept {
    return static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
  }
};

} /* namespace apex::container */

#endif /* APEX_CONTAINER_STRING_HPP */
