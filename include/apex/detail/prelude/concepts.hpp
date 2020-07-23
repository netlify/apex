#ifndef APEX_DETAIL_PRELUDE_CONCEPTS_HPP
#define APEX_DETAIL_PRELUDE_CONCEPTS_HPP

#include <apex/detail/prelude/traits.hpp>

#if __has_include(<concepts>)
  #include <concepts>
#else
  #include <type_traits>
  #include <new>
#endif /* __has_include(<concepts>) */

#if not APEX_CHECK_API(concepts, 202002)
namespace apex::detail::concepts {

template <class T, class U> concept similar_to = ::std::is_same_v<
  remove_cvref_t<T>,
  remove_cvref_t<U>
>;
template <class T, class U> concept same_as = ::std::is_same_v<T, U>;

} /* namespace apex::detail::concepts */
#endif /* not APEX_CHECK_API(concepts, 202002) */

namespace apex {

#if APEX_CHECK_API(concepts, 202002)

APEX_WARN("<concepts> is available in this implementation")
using ::std::default_initializable;
using ::std::move_constructible;
using ::std::constructible_from;
using ::std::destructible;
using ::std::movable;

using ::std::signed_integral;
using ::std::integral;

using ::std::convertible_to;
using ::std::same_as;
#else

template <class From, class To>
concept convertible_to = ::std::is_convertible_v<From, To>
  and requires (::std::add_rvalue_reference_t<From> (&f)()) {
    static_cast<To>(f());
  };

template <class T, class U>
concept same_as = detail::concepts::same_as<T, U>
  and detail::concepts::same_as<U, T>;

template <class T> concept integral = ::std::is_integral_v<T>;
template <class T> concept signed_integral = integral<T> and ::std::is_signed_v<T>;

template <class T> concept destructible = ::std::is_nothrow_destructible_v<T>;

template <class T, class... Args>
concept constructible_from = destructible<T>
  and ::std::is_constructible_v<T, Args...>;

// TODO: implement common_reference and friends
template <class T, class U>
concept assignable_from = ::std::is_lvalue_reference_v<T>
  and requires (T lhs, U rhs) {
    { lhs = static_cast<U&&>(rhs) } -> same_as<T>;
  };

template <class T>
concept default_initializable = constructible_from<T>
  and requires { T {}; }
  and requires (void* ptr) { ::new (ptr) T; };

template <class T>
concept move_constructible = constructible_from<T, T>
  and convertible_to<T, T>;

template <class T>
concept copy_constructible = move_constructible<T>
  and constructible_from<T, T&> and convertible_to<T&, T>
  and constructible_from<T, const T&> and convertible_to<const T&, T>
  and constructible_from<T, const T> and convertible_to<const T, T>;

template <class T>
concept movable = ::std::is_object_v<T> and move_constructible<T>;

template <class T>
concept copyable = copy_constructible<T>
  and movable<T>
  and assignable_from<T&, T const&>;

template <class T>
concept semiregular = copyable<T> and default_initializable<T>;

/* FIXME: This has been replaced with a boolean-testable exposition only
 * concept that is MUCH MORE involved. As a result, we may have to break up the
 * concepts header a bit more to make sure the dependencies are properly
 * handled.  See [concept.booleantestable] for more information on the (now
 * very complex) new definition
 */
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


/* weakly-equality-comparable-with */
template <class T, class U>
concept weakly_equality_comparable_with = requires (remove_cvref_t<T> const& x, remove_cvref_t<U> const& y) {
  { x == y } -> boolean;
  { x != y } -> boolean;
  { y == x } -> boolean;
  { y != x } -> boolean;
};


#endif /* APEX_CHECK_API(concepts, 202002) */

/* These are defined in P2199, but there's an issue regarding subsumption that
 * needs to be resolved. There's no guarantee that we'll get it right because
 * the definition of same_as might need to be redefined as a result :/
 * Also, the definitions for different_from and distinct_from are swapped,
 * because it makes more sense when read. This will be in P2199R1
 */
template <class T, class U>
concept similar_to = same_as<T, U>
  or (detail::concepts::similar_to<T, U> and detail::concepts::similar_to<U, T>);

template <class T, class U> concept different_from = not similar_to<T, U>;
template <class T, class U> concept distinct_from = not same_as<T, U>;

} /* namespace apex */

#endif /* APEX_DETAIL_PRELUDE_CONCEPTS_HPP */
