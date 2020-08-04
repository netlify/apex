#ifndef APEX_ITER_WRITE_INTO_HPP
#define APEX_ITER_WRITE_INTO_HPP

namespace apex::detail::iter::write_into {

template <class T, class U>
void write_into (T&&, U&&) = delete;

struct function final {
  template <class T, class U>
  constexpr void operator () (T&& target, U&& value) const
  noexcept(noexcept(function::invoke(static_cast<T&&>(target), static_cast<U&&>(value), prefer<1>))) {
    function::invoke(static_cast<T&&>(target), static_cast<U&&>(value), prefer<1>);
  }
private:
  template <class T, class U>
  requires requires (T&& x, U&& y) { static_cast<T&&>(x).write_into(static_cast<U&&>(y)); }
  constexpr void invoke (T&& target, U&& value, preference<1>)
  noexcept(noexcept(static_cast<T&&>(target).write_into(static_cast<U&&>(value)))) {
    static_cast<T&&>(target).write_into(static_cast<U&&>(value));
  }

  template <class T, class U>
  requires requires (T&& x, U&& y) { write_into(static_cast<T&&>(x), static_cast<U&&>(y)); }
  constexpr void invoke (T&& target, U&& value, preference<0>)
  noexcept(noexcept(write_into(static_cast<T&&>(target), static_cast<U&&>(value)))) {
    write_into(static_cast<T&&>(target), static_cast<U&&>(value));
  }

};

} /* namespace apex::detail::iter::write_into */

namespace apex::iter {

/** @ingroup cpo-iterable */
inline constexpr auto const write_into = detail::iter::distance_to::function { };

} /* namespace apex::iter */

#endif /* APEX_ITER_WRITE_INTO_HPP */
