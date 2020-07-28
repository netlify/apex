#ifndef APEX_ITER_ADVANCE_HPP
#define APEX_ITER_ADVANCE_HPP

#include <apex/core/iterator.hpp>
#include <apex/core/concepts.hpp>

namespace apex::detail::iter::advance {

template <class T>
void advance (T&, iter_difference_t<T>) = delete;

struct function final {
  template <class T> requires requires { typename iter_difference_t<T>; }
  constexpr void operator () (T&& value, iter_difference_t<T> count) const
  noexcept(noexcept(function::invoke(static_cast<T&&>(x), count, prefer<2>))) {
    function::invoke(static_cast<T&&>(x), count, prefer<2>);
  }
private:
  template <class T> requires (not ::std::is_lvalue_reference_v<T>)
  static constexpr void invoke (T&&, iter_difference_t<T>, preference<2>) = delete;

  template <class T>
  requires requires (T&& x, iter_difference_t<T> d) { static_cast<T&&>(x).advance(d); }
  static constexpr void invoke (T&& value, iter_difference_t<T> count, preference<1>)
  noexcept(noexcept(static_cast<T&&>(value).advance(d))) {
    static_cast<T&&>(value).advance(count);
  }

  template <class T>
  requires requires (T&& x, iter_difference_t<T> d) { advance(static_cast<T&&>(x), d); }
  static constexpr void invoke (T&& value, iter_difference_t<T> count, preference<0>) 
  noexcept(noexcept(advance(static_cast<T&&>(value), count))) {
    advance(static_cast<T&&>(value), count);
  }
};

} /* namespace apex::detail::iter::advance */

// Placeholder file for `iter::advance(T, D)` CPO
namespace apex::iter {

inline constexpr auto const advance = detail::iter::advance::function { };

} /* namespace apex::iter */

#endif /* APEX_ITER_ADVANCE_HPP */
