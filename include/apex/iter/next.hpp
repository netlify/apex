#ifndef APEX_ITER_NEXT_HPP
#define APEX_ITER_NEXT_HPP

#include <apex/iter/advance.hpp>

namespace apex::detail::iter::next {

template <class T> void next (T&) = delete;

struct function final {
  template <class T>
  constexpr void operator () (T&& value) const
  noexcept(noexcept(function::invoke(static_cast<T&&>(value), prefer<3>))) {
    return function::invoke(static_cast<T&&>(value), prefer<3>);
  }
private:
  template <class T> requires (not ::std::is_lvalue_reference_v<T>)
  static constexpr void invoke (T&&, preference<3>) = delete;

  template <class T>
  requires requires (T&& x) { static_cast<T&&>(x).next(); }
  static constexpr void invoke (T&& value, preference<2>)
  noexcept(noexcept(static_cast<T&&>(value).next())) {
    return static_cast<void>(static_cast<T&&>(value).next());
  }

  template <class T>
  requires requires (T&& x) { static_cast<void>(next(static_cast<T&&>(x))); }
  static constexpr void invoke (T&& value, preference<1>)
  noexcept(noexcept(next(static_cast<T&&>(value)))) {
    return static_cast<void>(next(static_cast<T&&>(value)));
  }

  template <class T>
  requires requires (T&& x) { ::apex::iter::advance(static_cast<T&&>(x), 1); }
  static constexpr void invoke (T&& value, preference<0>)
  noexcept(noexcept(::apex::iter::advance(static_cast<T&&>(value), 1))) {
    ::apex::iter::advance(static_cast<T&&>(value), 1);
  }
};

} /* namespace apex::detail::iter::next */

namespace apex::iter {

/** @var apex::iter::next
 * @brief Move an iterable to its successor.
 * @hideinitializer
 * @cpo{iterable}
 * @details The order of operations is to first call a member version of
 * `.%next()`, followed by an ADL lookup form of `next`, and lastly attempt to
 * call apex::iter::advance with `1`.
 */
inline constexpr auto const next = detail::iter::next::function { };

} /* namespace apex::iter */

#endif /* APEX_ITER_NEXT_HPP */
