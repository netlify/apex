#ifndef APEX_CORE_PRELUDE_HPP
#define APEX_CORE_PRELUDE_HPP

/**
 * The prelude can most easily be described as a set of declarations and
 * definitions that are required to be defined *everywhere* for the most basic
 * of operations.
 *
 * At this time, we do not have full access to C++20 library features. As a
 * result, we need to implement some of these ourselves. Because of the
 * complexity involved with implementing these 'shim' declarations, we need to
 * place them into their own headers elsewhere to make it manageable.
 * These can be found in `apex/detail/prelude/`. As features become more
 * available, we'll be able to delete these detail headers and move the
 * non-standard declarations back to this location.
 *
 * Of course, a lot of this goes away with modules, but that is a long ways
 * away, both in terms of standard support as well as support from CMake and
 * other build systems.
 */
#include <apex/detail/prelude/macros.hpp>
#include <apex/detail/prelude/traits.hpp>
#include <apex/detail/prelude/types.hpp>

#include <apex/detail/prelude/iterator.hpp>
#include <apex/detail/prelude/concepts.hpp>
#include <apex/detail/prelude/swap.hpp>

// IWYU pragma: begin_exports
#if not APEX_CHECK_API(concepts, 202002)
  #include <type_traits>
  #include <new>
#endif /* APEX_CHECK_API(concepts, 202002) */

// IWYU pragma: end_exports
//

/* This namespace fragment is used to declare common functions */
namespace apex {

/* This is typically an exposition only function, but it's used enough in
 * C++ that it *should* just be part of the standard
 * NOTE: We do not use std::forward because it turns out it's just faster
 * to use a static_cast and then we also don't need to #include <utility>
 */
template <class T>
constexpr ::std::decay_t<T> decay_copy (T&& value)
noexcept(::std::is_nothrow_convertible_v<T, ::std::decay_t<T>>) {
  return static_cast<T&&>(value);
}

} /* namespace apex */

/* This namespace fragment is used to declare the minimum set of concepts
 * required to create the *core* concepts used (including std::swappable)
 */
namespace apex {

// TODO: Remove this, as we won't need it in the future
template <class T>
concept dereferenceable = requires (T& t) { *t; };

#if not APEX_CHECK_API(ranges, 201911)
/* This is not technically correct because we do not use is-signed-integer-like,
 * but instead use signed_integral. That said we don't really *need* that, as
 * we aren't going to need anything beyond what is_signed_integral can provide us
 */
#endif /* APEX_CHECK_API(ranges, 201911) */

} /* namespace apex */

namespace apex::detail::types::begin {

template <class T>
concept member = requires (T&& t) {
  { static_cast<T&&>(t).begin() } -> input_or_output_iterator;
};

template <class T> void begin (T const&) = delete;
template <class T> void begin (T&) = delete;

class function {
  // TODO: requires allowing a "borrowed" range
  template <class T> requires (not ::std::is_lvalue_reference_v<T>)
  static constexpr void invoke (T&&, preference<3>) = delete;

  template <class T> requires ::std::is_array_v<remove_cvref_t<T>>
  static constexpr decltype(auto) invoke (T&& array, preference<2>) noexcept {
    return static_cast<T&&>(array) + 0;
  }

  template <member T>
  static constexpr decltype(auto) invoke (T&& value, preference<1>)
  noexcept(noexcept(apex::decay_copy(static_cast<T&&>(value).begin()))) {
    return apex::decay_copy(static_cast<T&&>(value).begin());
  }

  template <class T>
  static constexpr decltype(auto) invoke (T&& value, preference<0>)
  noexcept(noexcept(apex::decay_copy(begin(static_cast<T&&>(value))))) {
    return apex::decay_copy(begin(static_cast<T&&>(value)));
  }

public:
  template <class T>
  constexpr auto operator () (T&& value) const
  noexcept(noexcept(function::invoke(static_cast<T&&>(value), prefer<3>))) {
    return function::invoke(static_cast<T&&>(value), prefer<3>);
  }
};

} /* namespace apex::detail::types::begin */

namespace apex::detail::types::end {

template <class T> void end (T const&) = delete;
template <class T> void end (T&) = delete;

template <class T>
concept member = requires (T&& value, begin::function begin) {
  { static_cast<T&&>(value).end() } -> sentinel_for<decltype(begin(static_cast<T>(value)))>;
};

template <class T>
concept freeform = requires (T&& value, begin::function begin) {
  { end(static_cast<T&&>(value)) } -> sentinel_for<decltype(begin(static_cast<T>(value)))>;
};

class function {
  template <class T> requires (not ::std::is_lvalue_reference_v<T>)
  static constexpr void invoke (T&&, preference<3>) = delete;

  template <class T> requires ::std::is_array_v<remove_cvref_t<T>>
  static constexpr decltype(auto) invoke (T&& array, preference<2>) noexcept {
    return static_cast<T&&>(array) + ::std::extent_v<remove_cvref_t<T>>;
  }

  template <member T>
  static constexpr decltype(auto) invoke (T&& value, preference<1>)
  noexcept(noexcept(apex::decay_copy(static_cast<T&&>(value)))) {
    return apex::decay_copy(static_cast<T&&>(value).end());
  }

  template <freeform T>
  static constexpr decltype(auto) invoke (T&& value, preference<0>)
  noexcept(noexcept(apex::decay_copy(end(static_cast<T&&>(value))))) {
    return apex::decay_copy(end(static_cast<T&&>(value)));
  }
public:
  template <class T>
  constexpr auto operator () (T&& value) const
  noexcept(noexcept(function::invoke(static_cast<T&&>(value), prefer<3>))) {
    return function::invoke(static_cast<T&&>(value), prefer<3>);
  }
};

} /* namespace apex::detail::types::end */

#if not APEX_CHECK_API(ranges, 201911)
namespace apex::ranges {

[[clang::no_destroy]] inline constexpr auto const begin = detail::types::begin::function { };
[[clang::no_destroy]] inline constexpr auto const end = detail::types::end::function { };

} /* namespace apex::ranges */
#endif /* not APEX_CHECK_API(ranges, 201911) */

#endif /* APEX_CORE_PRELUDE_HPP */
