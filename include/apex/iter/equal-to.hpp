#ifndef APEX_ITER_EQUAL_TO_HPP
#define APEX_ITER_EQUAL_TO_HPP

#include <apex/iter/distance-to.hpp>
#include <apex/core/concepts.hpp>

namespace apex::detail::iter::equal_to {

template <class T, class U> bool equal_to (T&&, U&&) = delete;

struct function final {
  template <class T, class U>
  constexpr bool operator () (T&& value, U&& other) const
  noexcept(noexcept(function::invoke(static_cast<T&&>(value), static_cast<U&&>(other), prefer<3>))) {
    return function::invoke(static_cast<T&&>(value), static_cast<U&&>(other), prefer<3>);
  }
private:
  template <class T>
  requires requires (T&& x, T&& y) {
    { static_cast<T&&>(x).equal_to(static_cast<T&&>(y)) } -> convertible_to<bool>;
  } static constexpr auto invoke (T&& value, T&& other, preference<3>)
  noexcept(noexcept(static_cast<T&&>(value).equal_to(static_cast<T&&>(other)))) {
    return static_cast<T&&>(value).equal_to(static_cast<T&&>(other));
  }

  template <class T, class U>
  requires requires(T&& x, U&& y) {
    { equal_to(static_cast<T&&>(x), static_cast<T&&>(y)) } -> convertible_to<bool>;
  } static constexpr auto invoke (T&& value, U&& other, preference<2>)
  noexcept(noexcept(equal_to(static_cast<T&&>(value), static_cast<U&&>(other)))) {
    return equal_to(static_cast<T&&>(value), static_cast<U&&>(other));
  }

  template <class T>
  requires requires (T&& x, T&& y) {
    ::apex::iter::distance_to(static_cast<T&&>(x), static_cast<T&&>(y)) == 0;
  } static constexpr auto invoke (T&& value, T&& other, preference<1>)
  noexcept(noexcept(::apex::iter::distance_to(static_cast<T&&>(value), static_cast<T&&>(other)) == 0)) {
    return ::apex::iter::distance_to(static_cast<T&&>(value), static_cast<T&&>(other)) == 0;
  }

  template <class T, class U>
  requires requires (T&& x) {
    requires similar_to<typename T::sentinel_type, U>;
    { static_cast<T&&>(x).is_finished() } -> convertible_to<bool>;
  } static constexpr auto invoke (T&& value, U&&, preference<0>)
  noexcept(noexcept(static_cast<T&&>(value).is_finished())) {
    return static_cast<T&&>(value).is_finished();
  }
};

} /* namespace apex::detail::iter::equal_to */

namespace apex::iter {

/** @cpo{iterable} */
inline constexpr auto const equal_to = detail::iter::equal_to::function { };

} /* namespace apex::iter */

#endif /* APEX_ITER_EQUAL_TO_HPP */
