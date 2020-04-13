#ifndef APEX_ARRAY_HPP
#define APEX_ARRAY_HPP

#include <apex/macros.hpp>

#include <array>

#if not APEX_CHECK_API(to_array, 201907)
#include <functional>
#endif /* not APEX_CHECK_API(to_array, 201907) */


namespace apex {
inline namespace v1 {

#if APEX_CHECK_API(to_array, 201907)
using std::to_array;
#else
template <class T, std::size_t N>
constexpr std::array<std::remove_cv_t<T>, N> to_array (T (&a)[N]) {
  constexpr auto function = [](auto&&... args) constexpr {
    return std::array{args...};
  };
  return std::apply(function, a);
}

template <class T, std::size_t N>
constexpr std::array<std::remove_cv_t<T>, N> to_array (T (&&a)[N]) {
  constexpr auto function = [](auto&&... args) constexpr {
    return std::array { std::move(args)... };
  };
  return std::apply(function, a);
}
#endif /* APEX_CHECK_API(to_array) */

}} /* namespace apex::v1 */

#endif /* APEX_ARRAY_HPP */
