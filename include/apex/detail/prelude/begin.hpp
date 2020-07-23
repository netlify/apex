#ifndef APEX_DETAIL_PRELUDE_BEGIN_HPP
#define APEX_DETAIL_PRELUDE_BEGIN_HPP

#include <apex/detail/prelude/iterator.hpp>

#if not APEX_CHECK_API(ranges, 201911)

namespace apex::detail::prelude::begin {

template <class T> void begin (T const&) = delete;
template <class T> void begin (T&) = delete;

template <class T>
concept has_begin_member = requires (T&& t) {
  { static_cast<T&&>(t).begin() } -> input_or_output_iterator;
};

struct function {
private:
  template <class T> requires ::std::is_array_v<remove_cvref_t<T>>
  static constexpr decltype(auto) invoke (T&& array, preference<2>) noexcept {
    return static_cast<T&&>(array) + 0;
  }

};

} /* namespace apex::detail::prelude::begin */

namespace apex::ranges {

//[[clang::no_destroy]] inline constexpr auto const begin = detail::prelude::begin::function { };

} /* namespace apex::ranges */

#endif /* APEX_CHECK_API(ranges, 201911) */

#endif /* APEX_DETAIL_PRELUDE_BEGIN_HPP */
