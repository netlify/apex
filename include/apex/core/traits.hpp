#ifndef APEX_TRAITS_HPP
#define APEX_TRAITS_HPP

#include <type_traits>

#include <apex/core/macros.hpp>
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

template <class From, class To>
struct copy_lvalue_reference : conditional<
  is_lvalue_reference_v<From>,
  add_lvalue_reference_t<To>,
  To
> { };
template <class From, class To>
using copy_lvalue_reference_t = typename copy_lvalue_reference<From, To>::type;

template <class From, class To>
struct copy_rvalue_reference : conditional<
  is_rvalue_reference_v<From>,
  add_rvalue_reference_t<To>,
  To
> { };
template <class From, class To>
using copy_rvalue_reference_t = typename copy_rvalue_reference<From, To>::type;

template <class From, class To>
struct copy_reference :
  copy_lvalue_reference<From, copy_rvalue_reference_t<From, To>>
{ };
template <class From, class To>
using copy_reference_t = typename copy_reference<From, To>::type;

template <class From, class To>
struct copy_const : conditional<is_const_v<From>, add_const_t<To>, To> { };
template <class From, class To> using copy_const_t = typename copy_const<From, To>::type;

template <class From, class To>
struct copy_volatile : conditional<is_volatile_v<From>, add_volatile_t<To>, To> { };
template <class From, class To> using copy_volatile_t = typename copy_volatile<From, To>::type;

template <class From, class To>
struct copy_cv : copy_volatile<From, copy_const_t<From, To>> { };
template <class From, class To> using copy_cv_t = typename copy_cv<From, To>::type;

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

// TODO: place this into an impl::common_reference namespace instead 
// Put all common_reference related declarations into this namespace
// then just define common_reference with impl::common_reference::trait
// Alternatively, just use someone else's implementation and then cut the cruft out :/
#if not APEX_CHECK_API(concepts, 202002)
namespace impl {

template <class, class> struct common_ref;

template <class A, class B>
using meta_trans_other_3_6_t =  add_rvalue_reference_t<
  remove_reference_t<
    common_ref<
      remove_reference_t<A>,
      remove_reference_t<B>
    >
  >
>;

// [meta.trans.other]/3.1
template <class T>
using cref = add_lvalue_reference_t<remove_reference_t<T> const>;

// [meta.trans.other]/3.2
template <class A>
struct xref {
  template <class U>
  using T = copy_reference_t<A, copy_cv_t<remove_reference_t<A>, remove_cvref_t<U>>>;
};
// [meta.trans.other]/3.4
template <class X, class Y>
using cond_res = decltype(false ? std::declval<X(&)()>() : std::declval<Y(&)()>());

template <class A, class B>
concept meta_trans_other_3_5 = requires {
  requires is_lvalue_reference_v<A>;
  requires is_lvalue_reference_v<B>;
  typename cond_res<
    copy_cv_t<remove_reference_t<A>, remove_reference_t<B>>&,
    copy_cv_t<remove_reference_t<B>, remove_reference_t<A>>&
  >;
};

// [meta.trans.other]/3.5
template <class A, class B>
requires meta_trans_other_3_5<A, B>
struct common_ref<A, B> : type_identity<
  cond_res<
    copy_cv_t<remove_reference_t<A>, remove_reference_t<B>>&,
    copy_cv_t<remove_reference_t<B>, remove_reference_t<A>>&
  >
> { };

template <class A, class B>
concept meta_trans_other_3_6 = requires {
  typename meta_trans_other_3_6_t<A, B>;
  requires is_rvalue_reference_v<A>;
  requires is_rvalue_reference_v<B>;
  requires is_convertible_v<A, meta_trans_other_3_6_t<A, B>>;
  requires is_convertible_v<B, meta_trans_other_3_6_t<A, B>>;
};

// [meta.trans.other]/3.6
template <class A, class B>
requires meta_trans_other_3_6<A, B>
struct common_ref<A, B> : type_identity<meta_trans_other_3_6_t<A, B>> { };

// [meta.trans.other]/3.7
template <class A, class B>
requires is_rvalue_reference_v<A> and is_lvalue_reference_v<B> and requires {
  typename common_ref<cref<A>, remove_reference_t<B>&>;
  requires is_convertible_v<A, common_ref<cref<A>, remove_reference_t<B>&>>;
} struct common_ref<A, B> : common_ref<cref<A>, remove_reference_t<B>&> { };

// [meta.trans.other]/3.8
template <class A, class B>
requires is_lvalue_reference_v<A> and is_rvalue_reference_v<B>
struct common_ref<A, B> : common_ref<B, A> { };
} /* namespace impl */

template <class, class, template <class> class, template <class> class>
struct basic_common_reference { };
template <class...> struct common_reference;
template <class... Ts>
using common_reference_t = typename common_reference<Ts...>::type;

// [meta.trans.other]/4.1
template <> struct common_reference<> { };
// [meta.trans.other]/4.2
template <class T> struct common_reference<T> : common_type<T, T> { };
// [meta.trans.other]/4.3

// [meta.trans.other]/4.3.1
template <class T, class U>
requires requires {
  requires not is_same_v<T, decay_t<T>>;
  requires not is_same_v<U, decay_t<U>>;
} struct common_reference<T, U> :
  common_type<decay_t<T>, decay_t<U>>
{ };

// [meta.trans.other]/4.3.3
template <class T, class U>
requires requires {
  typename decay_t<decltype(false ? std::declval<decay_t<T>>() : std::declval<decay_t<U>>())>;
} struct common_reference<T, U> :
  decay<decltype(false ? std::declval<decay_t<T>>() : std::declval<decay_t<U>>())>
{ };

// [meta.trans.other]/4.3.4
template <class T, class U>
requires requires {
  typename impl::cond_res<impl::cref<decay_t<T>>, impl::cref<decay_t<U>>>;
} struct common_reference<T, U> :
  decay<impl::cond_res<impl::cref<decay_t<T>>, impl::cref<decay_t<U>>>>
{ };

// [meta.trans.other]/4.4
template <class T, class U, class V, class... Args>
struct common_reference<T, U, V, Args...> :
  common_reference<common_reference_t<T, U>, V, Args...>
{ };

#endif /* not APEX_CHECK_API(concepts, 202002) */

//template <class T, class U, class=void>
//struct common_reference<T, U, void> : std::common_type<T, U> { };

// We need to keep this around for implementing type traits at the concepts
// level, but that can wait.
template <class T, class, template <class...> class, class...>
struct detector : type_identity<T> { using value_t = std::false_type; };

template <class T, template <class...> class U, class... Args>
struct detector<T, std::void_t<U<Args...>>, U, Args...> :
  type_identity<U<Args...>>
{ using value_t = std::true_type; };

struct undefined final {
  undefined (undefined const&) = delete;
  undefined () = delete;
  ~undefined () = delete;
  void operator = (undefined const&) = delete;
};

template <class T, template <class...> class U, class... Args>
using detected_or = detector<T, void, U, Args...>;

template <template <class...> class T, class... Args>
using is_detected = typename detected_or<undefined, T, Args...>::value_t;

template <template <class...> class T, class... Args>
using detected_t = typename detected_or<undefined, T, Args...>::type;

template <class Default, template <class...> class Op, class... Args>
using detected_or_t = typename detected_or<Default, Op, Args...>::type;

template <class To, template <class...> class T, class... Args>
using is_detected_convertible = std::is_convertible<
  detected_t<T, Args...>,
  To
>;

template <class T, template <class...> class U, class... Args>
using is_detected_exact = std::is_same<T, detected_t<U, Args...>>;

template <template <class...> class T, class... Args>
inline constexpr bool is_detected_v = is_detected<T, Args...>::value;

template <class To, template <class...> class T, class... Args>
inline constexpr bool is_detected_convertible_v = is_detected_convertible<To, T, Args...>::value;

template <class T, template <class...> class U, class... Args>
inline constexpr bool is_detected_exect_v = is_detected_exact<T, U, Args...>::value;

// Custom type traits
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
inline constexpr auto constant = std::integral_constant<decltype(V), V> { };

template <class... Ts> inline constexpr auto always_false = false;

} /* namespace apex */

#endif /* APEX_TRAITS_HPP */
