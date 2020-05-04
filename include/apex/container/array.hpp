#ifndef APEX_CONTAINER_ARRAY_HPP
#define APEX_CONTAINER_ARRAY_HPP

#include <array>

#if __cpp_lib_to_array < 201907L
  #include <functional>
#endif /* __cpp_lib_to_array < 201907L */

namespace apex {
inline namespace v1 {

#if __cpp_lib_to_array >= 201907L
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
#endif /* __cpp_lib_to_array >= 201907L */

}} /* namespace apex::v1 */

#endif /* APEX_CONTAINER_ARRAY_HPP */
