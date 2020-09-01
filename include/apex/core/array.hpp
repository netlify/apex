#ifndef APEX_CORE_ARRAY_HPP
#define APEX_CORE_ARRAY_HPP

#include <apex/core/prelude.hpp>

// IWYU pragma: begin_exports
#include <array>

#if not APEX_CHECK_API(to_array, 201907)
  #include <functional>
  #include <utility>
#endif /* not APEX_CHECK_API(to_array, 201907) */
// IWYU pragma: end_exports

namespace apex {

#if APEX_CHECK_API(to_array, 201907)
using ::std::to_array;
#else
template <class T, ::std::size_t N>
constexpr ::std::array<::std::remove_cv_t<T>, N> to_array (T (&a)[N])
noexcept (::std::is_nothrow_copy_constructible_v<T>) {
  return [&]<::std::size_t... Is>(::std::index_sequence<Is...>) constexpr {
    return ::std::array { a[Is]... };
  }(::std::make_index_sequence<N>{});
}

template <class T, std::size_t N>
constexpr ::std::array<::std::remove_cv_t<T>, N> to_array (T (&&a)[N]) 
noexcept(::std::is_nothrow_move_constructible_v<T>) {
  using type = ::std::remove_reference_t<T>&&;
  return [&]<::std::size_t... Is>(::std::index_sequence<Is...>) constexpr {
    return ::std::array { static_cast<type>(a[Is])... };
  }(::std::make_index_sequence<N>{});
}
#endif /* APEX_CHECK_API(to_array, 201907) */

} /* namespace apex */

#endif /* APEX_CORE_ARRAY_HPP */
