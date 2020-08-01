#ifndef APEX_ITER_DISTANCE_TO_HPP
#define APEX_ITER_DISTANCE_TO_HPP

namespace apex::detail::iter::distance_to {

template <class T> void distance_to (T&&, T&&) = delete;

struct function final {
  template <class T>
  constexpr auto operator () (T&& value, T&& other) const
  noexcept(noexcept(function::invoke(static_cast<T&&>(value), static_cast<T&&>(other), prefer<1>))) {
    return function::invoke(static_cast<T&&>(value), static_cast<T&&>(other), prefer<1>);
  }
private:
  template <class T>
  requires requires (T&& value, T&& other) { static_cast<T&&>(value).distance_to(static_cast<T&&>(other)); }
  static constexpr auto invoke (T&& value, T&& other, preference<1>)
  noexcept(noexcept(static_cast<T&&>(value).distance_to(static_cast<T&&>(other)))) {
    return static_cast<T&&>(value).distance_to(other);
  }

  template <class T>
  requires requires (T&& x, T&& y) { distance_to(static_cast<T&&>(x), static_cast<T&&>(y)); }
  static constexpr auto invoke (T&& value, T&& other, preference<0>)
  noexcept(noexcept(distance_to(static_cast<T&&>(value), static_cast<T&&>(other)))) {
    return distance_to(static_cast<T&&>(value), static_cast<T&&>(other));
  }
};

} /* namespace apex::detail::iter::distance_to */

namespace apex::iter {

/** @brief Get the distance between two iterable types.
 * @bug Only same types are currently usable
 * @ingroup cpo-iterable
 */
inline constexpr auto const distance_to = detail::iter::distance_to::function { };

} /* namespace apex::iter */

#endif /* APEX_ITER_DISTANCE_TO_HPP */
