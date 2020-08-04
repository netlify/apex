#ifndef APEX_ITER_READ_FROM_HPP
#define APEX_ITER_READ_FROM_HPP

namespace apex::detail::iter::read_from {

template <class T> void read_from (T const&) = delete;

struct function final {
  template <class T>
  constexpr decltype(auto) operator () (T&& value) const
  noexcept(noexcept(function::invoke(static_cast<T&&>(value), prefer<1>))) {
    return function::invoke(static_cast<T&&>(value), prefer<1>);
  }
private:
  template <class T>
  requires requires (T&& x) { { static_cast<T&&>(x).read_from() } -> different_from<void>; }
  static constexpr decltype(auto) invoke (T&& value, preference<1>)
  noexcept(noexcept(static_cast<T&&>(value).read_from())) {
    return static_cast<T&&>(value).read_from();
  }

  template <class T>
  requires requires (T&& x) { { read_from(static_cast<T&&>(x)) } -> different_from<void>; }
  static constexpr decltype(auto) invoke (T&& value, preference<0>)
  noexcept(noexcept(read_from(static_cast<T&&>(value)))) {
    return read_from(static_cast<T&&>(value));
  }
};

} /* namespace apex::detail::iter::read_from */

namespace apex::iter {

/** @brief Read a value from the given parameter.
 * Member versions of `read_from` are called first, followed by ADL lookups
 * of `read_from`.
 * @note `read_from` is free to return references *or* values.
 * @ingroup cpo-iterable
 */
inline constexpr auto const read_from = detail::iter::read_from::function { };

} /* namespace apex::iter */

#endif /* APEX_ITER_READ_FROM_HPP */
