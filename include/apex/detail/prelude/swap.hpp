#ifndef APEX_DETAIL_PRELUDE_SWAP_HPP
#define APEX_DETAIL_PRELUDE_SWAP_HPP

#include <apex/detail/prelude/concepts.hpp>
#include <apex/detail/prelude/types.hpp>

#if not APEX_CHECK_API(ranges, 201911)
namespace apex::detail::prelude::swap {

template <class T, size_t N> void swap (T(&)[N], T(&)[N]) = delete;
template <class T> void swap (T&, T&) = delete;

struct function final {
  template <class T, class U>
  constexpr decltype(auto) operator () (T&& x, U&& y) const
  noexcept(noexcept(function::invoke(static_cast<T&&>(x), static_cast<U&&>(y), prefer<2>))) {
    return function::invoke(static_cast<T&&>(x), static_cast<U&&>(y), prefer<2>);
  }
private:
  // TODO: use a requires block
  template <class T, class U>
  requires requires (T&& x, U&& y) { static_cast<void>(swap(static_cast<T&&>(x), static_cast<U&&>(y))); }
  static constexpr decltype(auto) invoke (T&& x, U&& y, preference<2>)
  noexcept(noexcept(swap(static_cast<T&&>(x), static_cast<U&&>(y)))) {
    return static_cast<void>(swap(static_cast<T&&>(x), static_cast<U&&>(y)));
  }

  // TODO: technically needs to use ranges::swap_ranges, but that gets a bit
  // recursive 😬
  template <class T, class U, size_t N>
  requires requires (T (&x)[N], U(&y)[N], function& fn) { fn(*x, *y); }
  static constexpr auto invoke (T(&x)[N], U(&y)[N], preference<1>)
  noexcept(noexcept(std::declval<function&>()(*x, *y))) {
    for (size_t idx = 0; idx < N; ++idx) {
      function::invoke(x[idx], y[idx], prefer<2>);
    }
  }

  // FIXME: use assignable_from<T&, T> instead of is_assignable
  template <move_constructible T> /* requires assignable_from<T&, T> */
  requires ::std::is_assignable_v<T&, T>
  static constexpr auto invoke (T& a, T& b, preference<0>)
  noexcept(::std::is_nothrow_move_constructible_v<T> and ::std::is_nothrow_assignable_v<T&, T>) {
    auto temp = static_cast<::std::remove_reference_t<T>&&>(a);
    a = static_cast<::std::remove_reference_t<T>&&>(b);
    b = static_cast<::std::remove_reference_t<T>&&>(temp);
  }

};

} /* namespace apex::detail::prelude::swap */

namespace apex::ranges {

[[clang::no_destroy]] inline constexpr auto const swap = detail::prelude::swap::function { };

} /* namespace apex::ranges */
#endif /* APEX_CHECK_API(ranges, 201911) */


#endif /* APEX_DETAIL_PRELUDE_SWAP_HPP */
