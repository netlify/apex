#ifndef APEX_CORE_ARRAY_HPP
#define APEX_CORE_ARRAY_HPP

#include <apex/core/macros.hpp>
#include <array>

#if not APEX_CHECK_API(to_array, 201907)
  #include <functional>
#endif /* not APEX_CHECK_API(to_array, 201907) */

namespace apex {

#if APEX_CHECK_API(to_array, 201907)
using ::std::to_array;
#else
template <class T, std::size_t N>
constexpr std::array<std::remove_cv_t<T>, N> to_array (T (&a)[N])
noexcept (std::is_nothrow_copy_constructible_v<T>) {
  constexpr auto function = [] (auto&&... args) constexpr {
    return std::array { args... };
  };
  return std::apply(function, a);
}

template <class T, std::size_t N>
constexpr std::array<std::remove_cv_t<T>, N> to_array (T (&&a)[N]) 
noexcept(std::is_nothrow_move_constructible_v<T>) {
  constexpr auto function = [] (auto&&... args) constexpr {
    return std::array { std::move(args)... };
  };
  return std::apply(function, a);
}
#endif /* APEX_CHECK_API(to_array, 201907) */

} /* namespace apex */

#endif /* APEX_CORE_ARRAY_HPP */
