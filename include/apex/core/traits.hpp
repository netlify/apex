#ifndef APEX_CORE_TRAITS_HPP
#define APEX_CORE_TRAITS_HPP

// IWYU pragma: begin_exports
#include <type_traits>
// IWYU pragma: end_exports

#include <apex/core/prelude.hpp>
#include <apex/detect/types.hpp>

namespace apex {

// TODO: finish pulling in the rest of the std traits
using ::std::make_unsigned_t;
using ::std::make_signed_t;

using ::std::add_lvalue_reference_t;
using ::std::add_rvalue_reference_t;
using ::std::add_volatile_t;
using ::std::add_pointer_t;
using ::std::add_const_t;

using ::std::remove_reference_t;
using ::std::remove_pointer_t;
using ::std::remove_extent_t;
using ::std::remove_cv_t;
using ::std::decay_t;

using ::std::invoke_result_t;
using ::std::common_type_t;
using ::std::conditional_t;

using ::std::add_lvalue_reference;
using ::std::add_rvalue_reference;
using ::std::add_volatile;
using ::std::add_pointer;
using ::std::add_const;

using ::std::remove_reference;
using ::std::remove_pointer;
using ::std::remove_extent;
using ::std::remove_cv;
using ::std::decay;

using ::std::invoke_result;
using ::std::common_type;
using ::std::conditional;

using ::std::is_nothrow_copy_constructible;
using ::std::is_nothrow_move_constructible;
using ::std::is_nothrow_constructible;
using ::std::is_nothrow_destructible;
using ::std::is_nothrow_convertible;
using ::std::is_nothrow_assignable;
using ::std::is_nothrow_swappable;
using ::std::is_nothrow_invocable;

using ::std::is_default_constructible;
using ::std::is_move_constructible;
using ::std::is_constructible;
using ::std::is_assignable;
using ::std::is_swappable;

using ::std::is_lvalue_reference;
using ::std::is_rvalue_reference;
using ::std::is_convertible;
using ::std::is_assignable;
using ::std::is_function;
using ::std::is_volatile;
using ::std::is_object;
using ::std::is_scalar;
using ::std::is_array;
using ::std::is_const;
using ::std::is_void;
using ::std::is_same;

using ::std::conjunction;
using ::std::disjunction;
using ::std::negation;

using ::std::is_nothrow_copy_constructible_v;
using ::std::is_nothrow_move_constructible_v;
using ::std::is_nothrow_constructible_v;
using ::std::is_nothrow_destructible_v;
using ::std::is_nothrow_convertible_v;
using ::std::is_nothrow_assignable_v;
using ::std::is_nothrow_swappable_v;
using ::std::is_nothrow_invocable_v;

using ::std::is_default_constructible_v;
using ::std::is_move_constructible_v;
using ::std::is_constructible_v;
using ::std::is_assignable_v;
using ::std::is_swappable_v;

using ::std::is_lvalue_reference_v;
using ::std::is_rvalue_reference_v;
using ::std::is_convertible_v;
using ::std::is_assignable_v;
using ::std::is_function_v;
using ::std::is_volatile_v;
using ::std::is_object_v;
using ::std::is_scalar_v;
using ::std::is_array_v;
using ::std::is_const_v;
using ::std::is_void_v;
using ::std::is_same_v;

using ::std::conjunction_v;
using ::std::disjunction_v;
using ::std::negation_v;

// This is only available as an alias because it is used often for concepts,
// traits, and others machinery.
template <class T> using cref_t = add_lvalue_reference_t<add_const_t<remove_reference_t<T>>>;

/* shim section */
#if APEX_CHECK_API(type_identity, 201806)
  using ::std::type_identity_t;
  using ::std::type_identity;
#else
  template <class T> struct type_identity { using type = T; };
  template <class T> using type_identity_t = typename type_identity<T>::type;
#endif /* APEX_CHECK_API(type_identity, 201806) */

#if APEX_CHECK_API(bounded_array_traits, 201902)
  using ::std::is_unbounded_array;
  using ::std::is_bounded_array;

  using ::std::is_unbounded_array_v;
  using ::std::is_bounded_array_v;
#else
  template <class> struct is_unbounded_array : std::false_type { };
  template <class T> struct is_unbounded_array<T[]> : std::true_type { };
  template <class T> inline constexpr bool is_unbounded_array_v = is_unbounded_array<T>::value;

  template <class> struct is_bounded_array : std::false_type { };
  template <class T, std::size_t N> struct is_bounded_array<T[N]> : std::true_type { };
  template <class T> inline constexpr bool is_bounded_array_v = is_bounded_array<T>::value;
#endif /* APEX_CHECK_API(bounded_array_traits, 201902) */

// Custom type traits - some of these are also intended to be meta-functions
// for apex::detected
template <class> struct class_of;
template <class S, class T>
struct class_of<S T::*> : type_identity<T> { };
template <class T> using class_of_t = typename class_of<T>::type;

template <class T>
struct is_empty_base : std::conjunction<
  std::negation<std::is_final<T>>,
  std::is_empty<T>
> { };

template <class T>
inline constexpr auto is_empty_base_v = is_empty_base<T>::value;

template <class, template <class...> class>
struct is_specialization_of : std::false_type { };

template <template <class...> class T, class... Args>
struct is_specialization_of<T<Args...>, T> : std::true_type { };

template <class T, template <class...> class U>
inline constexpr auto is_specialization_of_v = is_specialization_of<T, U> { };

template <class, template <class, size_t> class>
struct is_bounded_specialization_of : std::false_type { };

template <template <class, size_t> class T, class U, size_t N>
struct is_bounded_specialization_of<T<U, N>, T> : std::true_type { };

template <class T, template <class, size_t> class U>
inline constexpr auto is_bounded_specialization_of_v = is_bounded_specialization_of<T, U> { };

template <auto V>
inline constexpr auto constant = ::std::integral_constant<decltype(V), V> { };

template <class... Ts> inline constexpr auto always_false = false;

} /* namespace apex */

#endif /* APEX_CORE_TRAITS_HPP */
