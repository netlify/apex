#ifndef APEX_CORE_CONCEPTS_HPP
#define APEX_CORE_CONCEPTS_HPP

#include <apex/core/prelude.hpp>
#include <apex/core/traits.hpp>
#include <functional>
#include <utility>
#include <new>

// Temporary shim file

#if __has_include(<concepts>)
  #include <concepts>
#endif /* __has_include(<concepts>) */

namespace apex {

#if APEX_CHECK_API(concepts, 202002)
  APEX_WARN("Concepts are now available. Please refactor.");
#else

/* core language */

template <class Derived, class Base>
concept derived_from =
  std::is_base_of_v<Base, Derived> and
  std::is_convertible_v<Derived volatile const*, Base volatile const*>;

template <class T, class U>
concept common_reference_with =
  same_as<common_reference_t<T, U>, common_reference_t<U, T>> and
  convertible_to<T, common_reference_t<T, U>> and
  convertible_to<U, common_reference_t<U, T>>;

template <class T, class U>
concept common_with = same_as<::std::common_type_t<T, U>, ::std::common_type_t<U, T>>
  and requires { 
    static_cast<::std::common_type_t<T, U>>(::std::declval<T>());
    static_cast<::std::common_type_t<T, U>>(::std::declval<U>());
  }
  and common_reference_with<
    ::std::add_lvalue_reference_t<T const>,
    ::std::add_lvalue_reference_t<U const>
  >
  and common_reference_with<
    ::std::add_lvalue_reference_t<::std::common_type_t<T, U>>,
    common_reference_t<
      ::std::add_lvalue_reference_t<T const>,
      ::std::add_lvalue_reference_t<U const>
    >
  >;

template <class T> concept unsigned_integral = integral<T> and not signed_integral<T>;
template <class T> concept floating_point = std::is_floating_point_v<T>;

template <class T>
concept swappable = requires (T& a, T& b) { ranges::swap(a, b); };
template <class T, class U>
concept swappable_with = common_reference_with<T, U> and requires (T&& x, U&& y) {
  ranges::swap(static_cast<T&&>(x), static_cast<T&&>(x));
  ranges::swap(static_cast<U&&>(y), static_cast<U&&>(y));
  ranges::swap(static_cast<T&&>(x), static_cast<U&&>(y));
  ranges::swap(static_cast<U&&>(y), static_cast<T&&>(x));
};

template <class T>
concept equality_comparable = weakly_equality_comparable_with<T, T>;

template <class T, class U>
concept equality_comparable_with = equality_comparable<T> 
  and equality_comparable<U>
  and common_reference_with<
    ::std::remove_reference_t<T> const&,
    ::std::remove_reference_t<U> const&
  > and equality_comparable<
    common_reference_t<
      ::std::remove_reference_t<T> const&,
      ::std::remove_reference_t<U> const&
    >
  > and weakly_equality_comparable_with<T, U>;

template <class T>
concept totally_ordered = equality_comparable<T>
  and requires (::std::remove_reference_t<T> const& a, ::std::remove_reference_t<T> const& b) {
    { a < b } -> boolean_testable;
    { a > b } -> boolean_testable;
    { a <= b } -> boolean_testable;
    { a >= b } -> boolean_testable;
  };

template <class T, class U>
concept totally_ordered_with = totally_ordered<T>
  and totally_ordered<U>
  and equality_comparable_with<T, U>
  and totally_ordered<common_reference_t<::std::remove_reference_t<T> const&, ::std::remove_reference_t<U> const&>>
  and requires (::std::remove_reference_t<T> const& t, ::std::remove_reference_t<U> const& u) {
    { t >= u } -> boolean_testable;
    { t <= u } -> boolean_testable;
    { u >= t } -> boolean_testable;
    { u <= t } -> boolean_testable;
    { t < u } -> boolean_testable;
    { t > u } -> boolean_testable;
    { u < t } -> boolean_testable;
    { u > t } -> boolean_testable;
  };

template <class T>
concept regular = semiregular<T> and equality_comparable<T>;

// callable
template <class F, class... Args>
concept invocable = requires (F&& f, Args&&... args) {
  ::std::invoke(static_cast<F&&>(f), static_cast<Args&&>(args)...);
};

template <class F, class... Args>
concept regular_invocable = invocable<F, Args...>;

template <class F, class... Args>
concept predicate = regular_invocable<F, Args...>
  and boolean<::std::invoke_result_t<F, Args...>>;

template <class R, class T, class U>
concept relation = predicate<R, T, T> and predicate<R, U, U>
  and predicate<R, T, U> and predicate<R, U, T>;

template <class R, class T, class U>
concept equivalence_relation = relation<R, T, U>;

template <class R, class T, class U>
concept strict_weak_order = relation<R, T, U>;

#endif /* APEX_CHECK_API(concepts, 202002) */

template <class T, class U>
concept addable_with = requires (T x, U y) {
  { x + y } -> different_from<void>;
};

template <class T, class U>
concept subtractable_with = requires (T x, U y) {
  { x - y } -> different_from<void>;
};

template <class T, class U>
concept multipliable_with = requires (T x, U y) {
  { x * y } -> different_from<void>;
};

template <class T, class U>
concept dividable_with = requires (T x, U y) {
  { x / y } -> different_from<void>;
};

template <class T>
concept postincrementable = requires (T x) {
  { x++ } -> different_from<void>;
};

template <class T>
concept postdecrementable = requires (T x) {
  { x-- } -> different_from<void>;
};

template <class T>
concept preincrementable = requires (T x) {
  { ++x } -> different_from<void>;
};

template <class T>
concept predecrementable = requires (T x) {
  { --x } -> different_from<void>;
};

// custom concepts
template <class T> concept enumeration = ::std::is_enum_v<T>;
template <class T> concept character = similar_to<char32_t, T>
  or similar_to<char16_t, T>
  or similar_to<char8_t, T>
  or similar_to<wchar_t, T>
  or similar_to<char, T>;

template <class T>
concept complete_type = requires {
  { sizeof(remove_cvref_t<remove_pointer_t<T>>) } -> same_as<size_t>;
};
template <class T>
concept incomplete_type = requires { requires not complete_type<T>; };

// If a type meets this, it is memcpy-able. We can use this for our bit_cast
// implementation.
template <class T> concept trivially_copyable = ::std::is_trivially_copyable_v<T>;
template <class T> concept complexly_copyable = not trivially_copyable<T>;
template <class T> concept standard_layout = ::std::is_standard_layout_v<T>;

template <class T> concept trivially_destructible = ::std::is_trivially_destructible_v<T>;
template <class T> concept complexly_destructible = not trivially_destructible<T>;

// TODO: This is not ready yet. We need to also enforce compound operators,
// addressof, boolean operators, modulo and (at some point) enforce a form
// of operator <=> if its defined.
template <class T>
concept bit_flag = enumeration<T> and requires (T t) {
  requires convertible_to<::std::underlying_type_t<T>, unsigned int>;
  requires not boolean<T>;

  requires not subtractable_with<T, ::std::underlying_type_t<T>>;
  requires not subtractable_with<T, T>;

  requires not multipliable_with<T, ::std::underlying_type_t<T>>;
  requires not multipliable_with<T, T>;

  requires not addable_with<T, ::std::underlying_type_t<T>>;
  requires not addable_with<T, T>;

  requires not dividable_with<T, ::std::underlying_type_t<T>>;
  requires not dividable_with<T, T>;

  requires not postincrementable<T>;
  requires not postdecrementable<T>;

  requires not preincrementable<T>;
  requires not predecrementable<T>;
};

} /* namespace apex */

#endif /* APEX_CORE_CONCEPTS_HPP */
