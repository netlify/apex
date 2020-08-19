#ifndef APEX_ITER_ADVANCE_HPP
#define APEX_ITER_ADVANCE_HPP

#include <apex/core/iterator.hpp>
#include <apex/core/concepts.hpp>

namespace apex::detail::iter::advance {

template <class T> void advance (T&, iter_difference_t<T>) = delete;

struct function final {
  template <class T> requires requires { typename iter_difference_t<T>; }
  constexpr void operator () (T&& value, iter_difference_t<T> offset) const
  noexcept(noexcept(function::invoke(static_cast<T&&>(value), offset, prefer<2>))) {
    function::invoke(static_cast<T&&>(value), offset, prefer<2>);
  }
private:
  template <class T> requires (not ::std::is_lvalue_reference_v<T>)
  static constexpr void invoke (T&&, iter_difference_t<T>, preference<2>) = delete;

  template <class T>
  requires requires (T&& x, iter_difference_t<T> d) { static_cast<T&&>(x).advance(d); }
  static constexpr void invoke (T&& value, iter_difference_t<T> offset, preference<1>)
  noexcept(noexcept(static_cast<T&&>(value).advance(offset))) {
    static_cast<T&&>(value).advance(offset);
  }

  template <class T>
  requires requires (T&& x, iter_difference_t<T> d) { advance(static_cast<T&&>(x), d); }
  static constexpr void invoke (T&& value, iter_difference_t<T> offset, preference<0>) 
  noexcept(noexcept(advance(static_cast<T&&>(value), offset))) {
    advance(static_cast<T&&>(value), offset);
  }
};

} /* namespace apex::detail::iter::advance */

namespace apex::iter {

/** @var apex::iter::advance
 * @cpo{iterable}
 * @hideinitializer
 * @brief Advance an iterable by some offset.
 * @details This function object will try to call (in order):
 *
 *  1. `value.advance(offset)`
 *  2. `advance(value, offset)`
 *
 * where the latter is subjugated to an ADL lookup.
 *
 * @param[in] value A reference type @c T
 * @param[in] offset A value of type `iter_difference_t<T>` to advance @p value by
 * @note rvalue references cannot be used with `apex::iter::advance`.
 */
inline constexpr auto const advance = detail::iter::advance::function { };

} /* namespace apex::iter */

#endif /* APEX_ITER_ADVANCE_HPP */
