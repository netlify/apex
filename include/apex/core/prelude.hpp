#ifndef APEX_CORE_PRELUDE_HPP
#define APEX_CORE_PRELUDE_HPP

/**
 * The prelude can most easily be described as a set of declarations and
 * definitions that are required to be defined *everywhere* for the most basic
 * of operations.
 * In several cases, we must use macros to 'shim' the design of some types.
 * Many of the definitions found in this file are technically found elsewhere,
 * such as within the `apex/core/concepts.hpp` header, or within
 * `apex/ranges/%.hpp`
 * We do *not* duplicate these types, but we *do* define them here so we can
 * break the need for cyclical headers. It should be noted that this issue
 * only goes away under C++20 Modules with the idea that we simply wouldn't
 * re-export these prelude declarations, *however* in practice we would still
 * have to define them within a prelude module, or with prelude module
 * fragments.
 *
 * NOTE: This is the ONLY FILE where we *do not* put headers at the very top,
 * but instead put our own macro preprocessor checks.
 * This is typically dangerous, but it's a risk we're going to take.
 */

/* This section is used to define *all* the most commonly used macros apex uses */
#if not defined(__has_cpp_attribute)
  #define __has_cpp_attribute(x) false
#endif /* not defined(__has_cpp_attribute) */

#if not defined(__has_builtin)
  #define __has_builtin(x) false
#endif /* not defined(__has_builtin) */

#define APEX_DETAIL_STRINGIFY(x) #x
#define APEX_STRINGIFY(x) APEX_IMPL_STRINGIFY(x)

#define APEX_CONCAT(x, y) APEX_PASTE(x, y)
#define APEX_PASTE(x, y) x##y

#define APEX_CHECK_ATTRIBIUTE(name) __has_cpp_attribute(name)
/* Clang does *NOT* like the use of APEX_CONCAT or APEX_PASTE */
#define APEX_CHECK_BUILTIN(name) (__has_builtin(__builtin_##name))

#define APEX_CHECK_API(name, version) (APEX_CONCAT(__cpp_lib_, name) >= APEX_CONCAT(version, L))
#define APEX_CHECK_CXX(name, version) (APEX_CONCAT(__cpp_, name) >= APEX_CONCAT(version, L))

#if __has_include(<version>)
  #include <version>
#endif /* __has_include(<version>) */

// IWYU pragma: begin_exports
#if not APEX_CHECK_API(concepts, 202002)
  #include <type_traits>
  #include <new>
#endif /* APEX_CHECK_API(concepts, 202002) */

#if not APEX_CHECK_API(ranges, 201911)
  #include <iterator>
#endif /* not APEX_CHECK_API(ranges, 201911) */

#include <cstddef>
#include <cstdint>
// IWYU pragma: end_exports

#if __has_include(<concepts>)
  #include <concepts>
#endif /* __has_include(<concepts>) */

/* This namespace fragment is used to declare common types and type aliases */
namespace apex {

using ::std::nullptr_t;
using ::std::ptrdiff_t;
using ::std::size_t;
using ::std::byte;

using i64 = ::std::int64_t;
using i32 = ::std::int32_t;
using i16 = ::std::int16_t;
using i8 = ::std::int8_t;

using u64 = ::std::uint64_t;
using u32 = ::std::uint32_t;
using u16 = ::std::uint16_t;
using u8 = ::std::uint8_t;

using f64 = double;
using f32 = float;

/* Turns out, having an empty struct is really useful! */
struct empty { };

/* Used for prioritizing possibly ambiguous overloads. I is the "level", where
 * higher numbers mean a higher level of preference
 */
template <size_t I> struct preference : preference<I - 1> { };
template <> struct preference<0> { };
template <size_t I> inline constexpr auto prefer = preference<I> { };

} /* namespace apex */

/* This namespace fragment is used to declare a few type traits that are needed */
namespace apex {

#if APEX_CHECK_API(remove_cvref, 201711)
  using ::std::remove_cvref_t;
  using ::std::remove_cvref;
#else
  template <class T>
  struct remove_cvref :
    ::std::remove_cv<::std::remove_reference_t<T>>
  { };

  template <class T> using remove_cvref_t = typename remove_cvref<T>::type;

#endif /* APEX_CHECK_API(remove_cvref, 201711) */

#if not APEX_CHECK_API(ranges, 201911)
template <class> struct incrementable_traits { };
template <class T>
struct incrementable_traits<T const> :
  incrementable_traits<T>
{ };
template <class T> requires (not ::std::is_object_v<T>)
struct incrementable_traits<T*> { };
template <class T> requires ::std::is_object_v<T>
struct incrementable_traits<T*> { using difference_type = ptrdiff_t; };
template <class T> requires requires { typename T::difference_type; }
struct incrementable_traits<T> {
  using difference_type = typename T::difference_type;
};
#endif /* not APEX_CHECK_API(ranges, 201911) */

} /* namespace apex */

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

/* This fragment is for some internal traits */
namespace apex::detail::traits {

template <class T>
struct iter_difference :
  std::iterator_traits<T>
{ };

template <class T>
requires requires { typename incrementable_traits<remove_cvref_t<T>>::difference_type; }
struct iter_difference<T> :
  incrementable_traits<T>
{ };


} /* namespace apex::detail::traits */

/* This fragment is for implementation detail types */
namespace apex::detail::types {

/* A custom implementation of std::ignore, placed into a 'smaller' header
 * so that we can use it without including the very heavy <tuple>
 */
struct ignore_t final {
  template <class T>
  constexpr ignore_t const& operator = (T&&) const noexcept { return *this; }
};

} /* namespace apex::detail::types */

/* This namespace fragment is used to declare exposition only concepts
 */
namespace apex::detail::concepts {

template <class T, class U> concept same_as = ::std::is_same_v<T, U>;

} /* namespace apex::detail::concepts */

/* This namespace fragment is used to declare the minimum set of concepts
 * required to create the *core* concepts used (including std::swappable)
 */
namespace apex {

#if APEX_CHECK_API(concepts, 202002)
#pragma GCC warning "Concepts appear to be available in this implementation!"
using ::std::default_initializable;
using ::std::movable;

using ::std::signed_integral;
using ::std::integral;
using ::std::same_as;
#else
template <class T, class U>
concept same_as = apex::detail::concepts::same_as<T, U> and apex::detail::concepts::same_as<U, T>;

template <class T> concept integral = ::std::is_integral_v<T>;
template <class T> concept signed_integral = integral<T> and ::std::is_signed_v<T>;

template <class T> concept destructible = ::std::is_nothrow_destructible_v<T>;

template <class From, class To>
concept convertible_to = ::std::is_convertible_v<From, To>
  and requires (::std::add_rvalue_reference_t<From> (&f)()) {
    static_cast<To>(f());
  };

template <class T, class... Args>
concept constructible_from = destructible<T>
  and ::std::is_constructible_v<T, Args...>;

template <class T>
concept default_initializable = constructible_from<T>
  and requires { T {}; }
  and requires (void* ptr) { ::new (ptr) T; };

template <class T>
concept move_constructible = constructible_from<T, T>
  and convertible_to<T, T>;

template <class T> concept copy_constructible = move_constructible<T>
  and constructible_from<T, T&>
  and convertible_to<T&, T> 
  and constructible_from<T, T const&>
  and constructible_from<T const&, T>
  and constructible_from<T const, T>
  and convertible_to<T const, T>;

// TODO: implement common_reference and then add it to this
template <class T, class U>
concept assignable_from = ::std::is_lvalue_reference_v<T>
  and requires (T lhs, U rhs) {
    { lhs = static_cast<U&&>(rhs) } -> same_as<T>;
  };

template <class T>
concept movable = ::std::is_object_v<T> and move_constructible<T>;

template <class T>
concept copyable = copy_constructible<T>
  and movable<T>
  and assignable_from<T&, T const&>;

template <class T>
concept semiregular = copyable<T> and default_initializable<T>;

template <class B>
concept boolean = movable<remove_cvref_t<B>>
  and requires (remove_cvref_t<B> const& x, remove_cvref_t<B> const& y, bool const b) {
    { x } -> convertible_to<bool>;
    { not x } -> convertible_to<bool>;
    { x and y } -> same_as<bool>;
    { x and b } -> same_as<bool>;
    { b and y } -> same_as<bool>;
    { x or y } -> same_as<bool>;
    { x or b } -> same_as<bool>;
    { b or y } -> same_as<bool>;
    { x == y } -> convertible_to<bool>;
    { x == b } -> convertible_to<bool>;
    { b == y } -> convertible_to<bool>;
    { x != y } -> convertible_to<bool>;
    { x != b } -> convertible_to<bool>;
    { b != y } -> convertible_to<bool>;
  };

template <class T, class U>
concept weak_equality_comparable_with = requires (remove_cvref_t<T> const& t, remove_cvref_t<U> const& u) {
  { t == u } -> boolean;
  { t != u } -> boolean;
  { u == t } -> boolean;
  { u != t } -> boolean;
};

#endif /* APEX_CHECK_API(concepts, 202002) */

// This is also typically known as not-same-as, and can-reference
template <class T, class U>
concept different_from = requires { requires not same_as<T, U>; };

template <class T, class U>
concept similar_to = same_as<remove_cvref_t<T>, remove_cvref_t<U>>;

template <class T, class U>
concept distinct_from = not similar_to<T, U>;

template <class T>
concept dereferenceable = requires (T& t) { *t; };

#if not APEX_CHECK_API(ranges, 201911)
template <class T>
using iter_difference_t = typename detail::traits::iter_difference<T>::type;

/* This is not technically correct because we do not use is-signed-integer-like,
 * but instead use signed_integral. That said we don't really *need* that, as
 * we aren't going to need anything beyond what is_signed_integral can provide us
 */
template <class I>
concept weakly_incrementable = default_initializable<I>
  and movable<I>
  and requires (I i) {
    typename iter_difference_t<I>;
    requires signed_integral<iter_difference_t<I>>;
    { ++i } -> same_as<I&>;
    i++;
  };

template <class I>
concept input_or_output_iterator = weakly_incrementable<I>
  and requires (I i) {
    { *i } -> distinct_from<void>;
  };

template <class S, class I>
concept sentinel_for = semiregular<S>
  and input_or_output_iterator<I>
  and weak_equality_comparable_with<S, I>;

#endif /* APEX_CHECK_API(ranges, 201911) */

} /* namespace apex */

/* This namespace fragment is used to take previously declared implementation
 * detail types and create inline constexpr variables of an instance of them
 */
namespace apex {

inline constexpr auto const ignore = detail::types::ignore_t { };

} /* namespace apex */

namespace apex::detail::types::swap {

template <class T, size_t N> void swap (T(&)[N], T(&)[N]) = delete;
template <class T> void swap (T&, T&) = delete;

class function {
  template <class T, class U>
  static constexpr auto invoke (T&& x, U&& y, preference<2>)
  noexcept(noexcept(swap(static_cast<T&&>(x), static_cast<U&&>(y)))) {
    return static_cast<void>(swap(static_cast<T&&>(x), static_cast<U&&>(y)));
  }

  template <class T, class U, size_t N>
  static constexpr void invoke (T(&t)[N], U(&u)[N], preference<1>)
  noexcept(noexcept(function::invoke(*t, *u, prefer<2>))) {
    for (size_t idx = 0; idx < N; ++idx) {
      function::invoke(t[idx], u[idx], prefer<2>);
    }
  }

  template <class T>
  requires ::std::is_move_constructible_v<T> and ::std::is_assignable_v<T&, T>
  static constexpr void invoke (T& a, T& b, preference<0>)
  noexcept(::std::is_nothrow_move_constructible_v<T> and ::std::is_nothrow_assignable_v<T&, T>) {
    using rvalue = ::std::remove_reference_t<T>&&;
    auto temp = static_cast<rvalue>(a);
    a = static_cast<rvalue>(b);
    b = static_cast<rvalue>(temp);
  }
public:
  template <class T, class U>
  constexpr decltype(auto) operator () (T&& x, U&& y) const
  noexcept(noexcept(function::invoke(static_cast<T&&>(x), static_cast<U&&>(y), prefer<2>))) {
    return function::invoke(static_cast<T&&>(x), static_cast<U&&>(y), prefer<2>);
  }
};

} /* namespace apex::detail::types::swap */

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

inline constexpr auto const begin = detail::types::begin::function { };
inline constexpr auto const swap = detail::types::swap::function { };
inline constexpr auto const end = detail::types::end::function { };

} /* namespace apex::ranges */
#else
#pragma GCC warning "Ranges are available in this implementation!"
#endif /* not APEX_CHECK_API(ranges, 201911) */

#endif /* APEX_CORE_PRELUDE_HPP */
