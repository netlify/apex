#ifndef APEX_CORE_CHARCONV_HPP
#define APEX_CORE_CHARCONV_HPP

#include <string_view>
#include <string>
#include <charconv>

#include <apex/core/types.hpp>

namespace apex {

using std::from_chars;

template <class T, class=std::enable_if_t<std::is_integral_v<T>>>
std::from_chars_result from_chars (char const* str, size_t len, T& value, int base = 10) noexcept {
  return from_chars(str, str + len, value, base);
}

template <class T, class=std::enable_if_t<std::is_integral_v<T>>>
std::from_chars_result from_chars (std::string const& str, T& value, int base = 10) noexcept {
  return from_chars(str.data(), str.size(), value, base);
}

template <class T, class=std::enable_if_t<std::is_integral_v<T>>>
std::from_chars_result from_chars (std::string_view str, T& value, int base = 10) noexcept {
  return from_chars(str.data(), str.size(), value, base);
}

} /* namespace apex */

#endif /* APEX_CORE_CHARCONV_HPP */
