#ifndef APEX_DETAIL_PRELUDE_BEGIN_HPP
#define APEX_DETAIL_PRELUDE_BEGIN_HPP

#include <apex/detail/prelude/decay-copy.hpp>
#include <apex/detail/prelude/iterator.hpp>

#if not APEX_CHECK_API(ranges, 201911)

namespace apex::detail::prelude::begin {

template <class T> void begin (T const&) = delete;
template <class T> void begin (T&) = delete;

template <class T>
concept has_begin_member = requires (T&& t) {
  { ::apex::detail::prelude::decay_copy(static_cast<T&&>(t).begin()) } -> input_or_output_iterator;
};

template <class T>
concept has_begin_adl = requires (T&& t) {
  { ::apex::detail::prelude::decay_copy(begin(static_cast<T&&>(t))) } -> input_or_output_iterator;
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
    return static_cast<T&&>(array) + 0;
  }

  template <class T> requires has_begin_member<T>
  static constexpr decltype(auto) invoke (T&& x, preference<1>)
  noexcept(noexcept(::apex::detail::prelude::decay_copy(static_cast<T&&>(x).begin()))) {
    return ::apex::detail::prelude::decay_copy(static_cast<T&&>(x).begin());
  }

  template <class T> requires has_begin_adl<T>
  static constexpr decltype(auto) invoke (T&& x, preference<0>)
  noexcept(noexcept(::apex::detail::prelude::decay_copy(begin(static_cast<T&&>(x))))) {
    return ::apex::detail::prelude::decay_copy(begin(static_cast<T&&>(x)));
  }
};

} /* namespace apex::detail::prelude::begin */

namespace apex::ranges {

[[clang::no_destroy]] inline constexpr auto const begin = detail::prelude::begin::function { };

} /* namespace apex::ranges */
#else
#include <ranges>
namespace apex { namespace ranges = ::std::ranges; } /* namespace apex */
#endif /* APEX_CHECK_API(ranges, 201911) */

#endif /* APEX_DETAIL_PRELUDE_BEGIN_HPP */
