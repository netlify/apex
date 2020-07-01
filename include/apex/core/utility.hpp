#ifndef APEX_CORE_UTILITY_HPP
#define APEX_CORE_UTILITY_HPP

#include <apex/core/concepts.hpp>

#include <utility>
#include <cstring>

// gotta love writing shims for features we don't have :(
#if not APEX_CHECK_API(integer_comparison_functions, 202002)
namespace apex::impl {

template <class T>
concept cmp_integer = integral<T> and requires {
  requires not same_as<remove_cvref_t<T>, std::byte>;
  requires not same_as<remove_cvref_t<T>, char32_t>;
  requires not same_as<remove_cvref_t<T>, char16_t>;
  #if APEX_CHECK_CXX(char8_t, 201811)
  requires not same_as<remove_cvref_t<T>, char8_t>;
  #endif /* APEX_CHECK_CXX(char8_t, 201811) */
  requires not same_as<remove_cvref_t<T>, wchar_t>;
  requires not same_as<remove_cvref_t<T>, char>;
  requires not same_as<remove_cvref_t<T>, bool>;
};

} /* namespace apex::impl */
#endif /* not APEX_CHECK_API(integer_comparison_functions, 202002) */

namespace apex {

using std::declval;

template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

// TODO: add enumeration concept
template <class T> requires std::is_enum_v<T>
constexpr auto to_underlying (T value) noexcept -> std::underlying_type_t<T> {
  return static_cast<std::underlying_type_t<T>>(value);
}

// NOTE: The paper this is for has undergone some feedback, including a new
// name that the author wasn't present for and couldn't defend. We're sticking
// with offset_to. Additionally, it wasn't given a specific header/module
// location. For this reason, it's placed inside the <apex/core/utility.hpp> header
/*
 * This is relies on implementation specific behavior.
 * We assume Windows/macOS/Linux ABIs
 *
 * TODO: move to either ffi or abi 'module' (not yet created)
 */
template <class T>
requires std::is_member_object_pointer_v<T> and std::is_standard_layout_v<class_of_t<T>>
ptrdiff_t offset_to (T const& member) noexcept {
  constexpr bool valid = sizeof(T) == sizeof(ptrdiff_t) or
    sizeof(T) == sizeof(i32);
  static_assert(valid, "Cannot safely get offset of member pointer");
  using value_type = conditional_t<
    sizeof(T) == sizeof(ptrdiff_t),
    ptrdiff_t,
    i32
  >;
  value_type value { };
  std::memcpy(&value, &member, sizeof(value));
  return value;
}

#if APEX_CHECK_API(integer_comparison_functions, 202002)
  using std::cmp_greater_equal;
  using std::cmp_less_equal;
  using std::cmp_greater;
  using std::cmp_less;
  using std::in_range;
#else
template <class T, class U> requires impl::cmp_integer<T>
constexpr bool cmp_equal (T t, U u) noexcept {
  if constexpr (std::is_signed_v<T> == std::is_signed_v<U>) { return t == u; }
  else if constexpr (std::is_signed_v<T>) {
    return t < 0 ? false : static_cast<std::make_unsigned_t<T>>(t) == u;
  } else {
    return u < 0 ? false : t == static_cast<std::make_unsigned_t<U>>(u);
  }
}

template <class T, class U>
constexpr bool cmp_not_equal(T t, U u) noexcept { return not cmp_equal(t, u); }

template <class T, class U> requires impl::cmp_integer<T>
constexpr bool cmp_less (T t, U u) noexcept {
  if constexpr (std::is_signed_v<T> == std::is_signed_v<U>) {
    return t < u;
  } else if constexpr (std::is_signed_v<T>) {
    return t < 0 ? true : static_cast<std::make_unsigned_t<T>>(t) < u;
  } else {
    return u < 0 ? false : t < static_cast<std::make_unsigned_t<U>>(u);
  }
}

template <class T, class U>
constexpr bool cmp_greater (T t, U u) noexcept {
  return cmp_less(u, t);
}

template <class T, class U>
constexpr bool cmp_less_equal (T t, U u) noexcept {
  return not cmp_greater(t, u);
}

template <class T, class U>
constexpr bool cmp_greater_equal (T t, U u) noexcept {
  return not cmp_less(t, u);
}

template <class R, class T>
constexpr bool in_range (T t) noexcept {
  return apex::cmp_greater_equal(t, std::numeric_limits<R>::min()) and
    apex::cmp_less_equal(t, std::numeric_limits<R>::max());
}

#endif /* APEX_CHECK_API(integer_comparison_functions, 202002) */

} /* namespace apex */

#endif /* APEX_CORE_UTILITY_HPP */
