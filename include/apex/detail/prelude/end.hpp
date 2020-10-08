#ifndef APEX_DETAIL_PRELUDE_END_HPP
#define APEX_DETAIL_PRELUDE_END_HPP

#include <apex/detail/prelude/decay-copy.hpp>
#include <apex/detail/prelude/iterator.hpp>

#if not APEX_CHECK_API(ranges, 201911)

namespace apex::detail::prelude::end {

template <class T> void end (T const&) = delete;
template <class T> void end (T&) = delete;

template <class T>
concept has_end_member = requires (T&& t) {
  { ::apex::detail::prelude::decay_copy(static_cast<T&&>(t).end()) } -> input_or_output_iterator;
};

template <class T>
concept has_end_adl = requires (T&& t) {
  { ::apex::detail::prelude::decay_copy(end(static_cast<T&&>(t))) } -> input_or_output_iterator;
};

struct function final {
  template <class T>
  constexpr decltype(auto) operator () (T&& x) const
  noexcept(noexcept(function::invoke(static_cast<T&&>(x), prefer<2>))) {
    return function::invoke(static_cast<T&&>(x), prefer<2>);
  }

private:
  template <class T> requires ::std::is_array_v<remove_cvref_t<T>>
  static constexpr decltype(auto) invoke (T&& array, preference<2>) noexcept {
    return static_cast<T&&>(array) + ::std::extent_v<::std::remove_reference_t<T>>;
  }

  template <class T> requires has_end_member<T>
  static constexpr decltype(auto) invoke (T&& x, preference<1>)
  noexcept(noexcept(::apex::detail::prelude::decay_copy(static_cast<T&&>(x).end()))) {
    return ::apex::detail::prelude::decay_copy(static_cast<T&&>(x).end());
  }

  template <class T> requires has_end_adl<T>
  static constexpr decltype(auto) invoke (T&& x, preference<0>)
  noexcept(noexcept(::apex::detail::prelude::decay_copy(end(static_cast<T&&>(x))))) {
    return ::apex::detail::prelude::decay_copy(end(static_cast<T&&>(x)));
  }
};

} /* namespace apex::detail::prelude::end */

namespace apex::ranges {

[[clang::no_destroy]] inline constexpr auto const end = detail::prelude::end::function { };

} /* namespace apex::ranges */
#else
#include <ranges>
namespace apex { namespace ranges = std::ranges; } /* namespace apex */
#endif /* not APEX_CHECK_API(ranges, 201911) */

#endif /* APEX_DETAIL_PRELUDE_END_HPP */
