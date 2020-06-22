#ifndef APEX_CORE_CONCEPTS_HPP
#define APEX_CORE_CONCEPTS_HPP

#include <apex/core/traits.hpp>
#include <functional>
#include <utility>
#include <new>

// Temporary shim file

#if __has_include(<concepts>)
  #include <concepts>
#endif /* __has_include(<concepts>) */

#if not APEX_CHECK_API(concepts, 202002)
namespace apex::impl {

template <class T, class U> concept same_as = is_same_v<T, U>;

} /* namespace impl */
#endif /* not APEX_CHECK_API(concepts, 202002) */

#if not APEX_CHECK_API(concepts, 202002)
// TODO: This needs to be moved into a core/ranges.hpp file
namespace apex::detail {

template <class T, size_t N> void swap (T(&)[N], T(&)[N]) = delete;
template <class T> void swap (T&, T&) = delete;

struct swappable {
  template <class T, class U>
  constexpr decltype(auto) operator () (T&& t, U&& u) const
  noexcept(noexcept(swappable::call(std::forward<T>(t), std::forward<U>(u), prefer<2>))) {
    return swappable::call(std::forward<T>(t), std::forward<U>(u), prefer<2>);
  }
private:
  // TODO: these could just use requires... couldn't it? 
  template <class T, class U>
  static constexpr auto call (T&& t, U&& u, preference<2>)
  noexcept(noexcept(swap(std::forward<T>(t), std::forward<U>(u)))) {
    return static_cast<void>(swap(std::forward<T>(t), std::forward<U>));
  }

  template <class T, class U, size_t N, class S = swappable>
  static constexpr void call (T (&t)[N], U(&u)[N], preference<1>)
  noexcept(noexcept(declval<S&>()(*t, *u))) {
    for (size_t idx = 0; idx < N; ++idx) {
      swappable::call(t[idx], u[idx], prefer<2>);
    }
  }

  template <class T>
  requires std::is_move_constructible_v<T> and std::is_assignable_v<T&, T>
  static constexpr void call (T& a, T& b, preference<0>)
  noexcept(is_nothrow_move_constructible_v<T> and std::is_nothrow_assignable_v<T&, T>) {
    auto temp = std::move(a);
    a = std::move(b);
    b = std::move(temp);
  }
};

} /* namespace apex::detail */

namespace apex::ranges {

inline constexpr auto swap = detail::swappable { };

} /* namespace apex::ranges */

#endif /* not APEX_CHECK_API(concepts, 202002) */

namespace apex {

#if APEX_CHECK_API(concepts, 202002)
/* core language */
using ::std::same_as;
using ::std::derived_from;
using ::std::convertible_to;
using ::std::common_reference_with;
using ::std::common_with;
using ::std::integral;
using ::std::signed_integral;
using ::std::unsigned_integral;
using ::std::floating_point;
using ::std::assignable_from;
using ::std::swappable;
using ::std::swappable_with;
using ::std::destructible;
using ::std::constructible_from;
using ::std::default_initializable;
using ::std::move_constructible;
using ::std::copy_constructible;
/* comparison */
using ::std::boolean;
using ::std::equality_comparable;
using ::std::equality_comparable_with;
using ::std::totally_ordered;
using ::std::totally_ordered_with;
/* object */
using ::std::movable;
using ::std::copyable;
using ::std::semiregular;
using ::std::regular;
/* callable */
using ::std::invocable;
using ::std::regular_invocable;
using ::std::predicate;
using ::std::relation;
using ::std::equivalence_relation;
using ::std::strick_weak_order;
#else

/* core language */

template <class T, class U>
concept same_as = impl::same_as<T, U> and impl::same_as<U, T>;

template <class Derived, class Base>
concept derived_from =
  std::is_base_of_v<Base, Derived> and
  std::is_convertible_v<Derived volatile const*, Base volatile const*>;

template <class From, class To>
concept convertible_to = std::is_convertible_v<From, To> and
  requires (std::add_rvalue_reference_t<From> (&f)()) {
    static_cast<To>(f());
  };

template <class T, class U>
concept common_reference_with =
  same_as<common_reference_t<T, U>, common_reference_t<U, T>> and
  convertible_to<T, common_reference_t<T, U>> and
  convertible_to<U, common_reference_t<U, T>>;

template <class T> concept integral = std::is_integral_v<T>;
template <class T> concept signed_integral = integral<T> and std::is_signed_v<T>;
template <class T> concept unsigned_integral = integral<T> and not signed_integral<T>;
template <class T> concept floating_point = std::is_floating_point_v<T>;

template <class T> concept destructible = is_nothrow_destructible_v<T>;
template <class T, class... Args> concept constructible_from =
  destructible<T> and std::is_constructible_v<T, Args...>;
template <class T> concept default_initializable = constructible_from<T>
  and requires { T {}; }
  and requires (void* ptr) { ::new (ptr) T; };
template <class T> concept move_constructible =
  constructible_from<T, T> and
  convertible_to<T, T>;
template <class T> concept copy_constructible =
  move_constructible<T> and
  constructible_from<T, T&> and convertible_to<T&, T> and
  constructible_from<T, T const&> and convertible_to<T const&, T> and
  constructible_from<T, T const> and convertible_to<T const, T>;
// TODO: implement common_reference_with
template <class LHS, class RHS>
concept assignable_from = is_lvalue_reference_v<LHS>
  and is_assignable_v<LHS, RHS>
  and common_reference_with<cref_t<LHS>, cref_t<RHS>>
  and requires (LHS lhs, RHS&& rhs) {
    { lhs = std::forward<RHS>(rhs) } -> same_as<LHS>;
  };
// NOTE: This is currently incorrect, but we don't have ranges::swap at the moment
template <class T>
concept swappable = requires (T& a, T& b) { ranges::swap(a, b); };
//template <class T, class U>
//concept swappable_with = common_reference_with<T, U> and requires (T&& t, U&& u) {
//  std::swap(std::forward<T>(t), std::forward<T>(t));
//};

// object
template <class T>
concept movable = std::is_object_v<T> and move_constructible<T>;

// comparison
template <class B>
concept boolean = movable<remove_cvref_t<B>>
  and requires (cref_t<B> b1, cref_t<B> b2, const bool a) {
    { b1 } -> convertible_to<bool>;
    { not b1 } -> convertible_to<bool>;
    { b1 and b2 } -> same_as<bool>;
    { b1 and a } -> same_as<bool>;
    { a and b2 } -> same_as<bool>;
    { b1 or b2 } -> same_as<bool>;
    { b1 or a } -> same_as<bool>;
    { a or b2 } -> same_as<bool>;
    { b1 == b2 } -> convertible_to<bool>;
    { b1 == a } -> convertible_to<bool>;
    { a == b2 } -> convertible_to<bool>;
    { b1 != b2 } -> convertible_to<bool>;
    { b1 != a } -> convertible_to<bool>;
    { a != b2 } -> convertible_to<bool>;
  };

template <class T, class U>
concept weak_equality_comparable_with = requires (cref_t<T> t, cref_t<U> u) {
  { t == u } -> boolean;
  { t != u } -> boolean;
  { u == t } -> boolean;
  { u != t } -> boolean;
};

template <class T>
concept equality_comparable = weak_equality_comparable_with<T, T>;

template <class T, class U>
concept equality_comparable_with = equality_comparable<T> 
  and equality_comparable<U>
  and common_reference_with<cref_t<T>, cref_t<U>>
  and equality_comparable<common_reference_t<cref_t<T>, cref_t<U>>>
  and weak_equality_comparable_with<T, U>;

template <class T>
concept totally_ordered = equality_comparable<T>
  and requires (cref_t<T> a, cref_t<T> b) {
    { a < b } -> boolean;
    { a > b } -> boolean;
    { a <= b } -> boolean;
    { a >= b } -> boolean;
  };

template <class T, class U>
concept totally_ordered_with = totally_ordered<T>
  and totally_ordered<U>
  and equality_comparable_with<T, U>
  and totally_ordered<common_reference_t<cref_t<T>, cref_t<U>>>
  and requires (cref_t<T> t, cref_t<U> u) {
    { t >= u } -> boolean;
    { t <= u } -> boolean;
    { u >= t } -> boolean;
    { u <= t } -> boolean;
    { t < u } -> boolean;
    { t > u } -> boolean;
    { u < t } -> boolean;
    { u > t } -> boolean;
  };

// object
template <class T>
concept copyable = copy_constructible<T>
  and movable<T>
  and assignable_from<T&, T const&>;

template <class T>
concept semiregular = copyable<T> and default_initializable<T>;

template <class T>
concept regular = semiregular<T> and equality_comparable<T>;

// callable
template <class F, class... Args>
concept invocable = requires (F&& f, Args&&... args) {
  std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
};

template <class F, class... Args>
concept regular_invocable = invocable<F, Args...>;

template <class F, class... Args>
concept predicate = regular_invocable<F, Args...>
  and boolean<std::invoke_result_t<F, Args...>>;

template <class R, class T, class U>
concept relation = predicate<R, T, T> and predicate<R, U, U>
  and predicate<R, T, U> and predicate<R, U, T>;

template <class R, class T, class U>
concept equivalence_relation = relation<R, T, U>;

template <class R, class T, class U>
concept strict_weak_order = relation<R, T, U>;

#endif /* APEX_CHECK_API(concepts, 202002) */

// custom concepts
template <class T>
concept complete_type = requires {
  { sizeof(remove_cvref_t<remove_pointer_t<T>>) } -> same_as<size_t>;
};
template <class T>
concept incomplete_type = requires { requires not complete_type<T>; };

template <class T, class U>
concept different_from = requires { requires not same_as<T, U>; };

// If a type meets this, it is memcpy-able. We can use this for our bit_cast
// implementation.
template <class T> concept trivially_copyable = std::is_trivially_copyable_v<T>;
template <class T> concept nontrivially_copyable = not trivially_copyable<T>;
template <class T> concept standard_layout = std::is_standard_layout_v<T>;


} /* namespace apex */

#endif /* APEX_CORE_CONCEPTS_HPP */
