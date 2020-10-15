#ifndef APEX_DETAIL_PRELUDE_COMMON_REFERENCE_HPP
#define APEX_DETAIL_PRELUDE_COMMON_REFERENCE_HPP

#include <apex/detail/prelude/remove-cvref.hpp>
#include <apex/detail/prelude/detected.hpp>


#if not APEX_CHECK_API(concepts, 202002) or APEX_USES_LIBSTDCXX

namespace apex::detail::prelude {

template <class, class> struct common_ref;

template <class From, class To>
struct copy_lvalue_reference : ::std::conditional<
  ::std::is_lvalue_reference_v<From>,
  ::std::add_lvalue_reference_t<To>,
  To
> { };
template <class From, class To>
struct copy_rvalue_reference : ::std::conditional<
  ::std::is_rvalue_reference_v<From>,
  ::std::add_rvalue_reference_t<To>,
  To
> { };

template <class From, class To>
struct copy_reference :
  copy_lvalue_reference<From, typename copy_rvalue_reference<From, To>::type>
{ };
template <class From, class To>
struct copy_volatile : ::std::conditional<
  ::std::is_volatile_v<From>,
  ::std::add_volatile_t<To>,
  To
 > { };

template <class From, class To>
struct copy_const :
  ::std::conditional<::std::is_const_v<From>, ::std::add_const_t<To>, To>
{ };

template <class From, class To>
struct copy_cv :
  copy_volatile<From, typename copy_const<From, To>::type>
{ };

template <class From, class To>
using copy_lvalue_reference_t = typename copy_lvalue_reference<From, To>::type;
template <class From, class To>
using copy_rvalue_reference_t = typename copy_rvalue_reference<From, To>::type;
template <class From, class To>
using copy_reference_t = typename copy_reference<From, To>::type;
template <class From, class To> using copy_volatile_t = typename copy_volatile<From, To>::type;
template <class From, class To> using copy_const_t = typename copy_const<From, To>::type;
template <class From, class To> using copy_cv_t = typename copy_cv<From, To>::type;

template <class A, class B>
using meta_trans_other_3_6_type = ::std::add_rvalue_reference<
  ::std::remove_reference_t<
    common_ref<
      ::std::remove_reference_t<A>,
      ::std::remove_reference_t<B>
    >
  >
>;

template <class T, class U>
using meta_trans_other_4_3_3_type = ::std::decay<
  decltype(false 
    ? ::std::declval<::std::decay_t<T>>()
    : ::std::declval<::std::decay_t<U>>())>;

// [meta.trans.other]/3.1
template <class T>
using cref = ::std::add_lvalue_reference_t<
  ::std::add_const_t<::std::remove_reference_t<T>>
>;

// [meta.trans.other]/3.2
template <class A>
struct xref {
  template <class U>
  using type = copy_reference_t<A, copy_cv_t<::std::remove_reference_t<A>, remove_cvref_t<U>>>;
};

// [meta.trans.other]/3.4
template <class X, class Y>
using cond_res = decltype(false
  ? ::std::declval<X(&)()>()()
  : ::std::declval<Y(&)()>()());

template <class A, class B>
concept meta_trans_other_3_5 = ::std::is_lvalue_reference_v<A>
  and ::std::is_lvalue_reference_v<B>
  and requires {
    typename cond_res<
      copy_cv_t<::std::remove_reference_t<A>, ::std::remove_reference_t<B>>&,
      copy_cv_t<::std::remove_reference_t<B>, ::std::remove_reference_t<A>>&
    >;
  };

template <class A, class B>
concept meta_trans_other_3_6 = ::std::is_rvalue_reference_v<A>
  and ::std::is_rvalue_reference_v<B>
  and requires {
    requires ::std::is_convertible_v<A, typename meta_trans_other_3_6_type<A, B>::type>;
    requires ::std::is_convertible_v<B, typename meta_trans_other_3_6_type<A, B>::type>; 
  };

template <class A, class B>
concept meta_trans_other_3_7 = ::std::is_rvalue_reference_v<A>
  and ::std::is_lvalue_reference_v<B>
  and requires {
    ::std::is_convertible_v<A, common_ref<cref<A>, ::std::remove_reference_t<B>&>>;
  };

template <class A, class B>
concept meta_trans_other_3_8 = ::std::is_lvalue_reference_v<A>
  and ::std::is_rvalue_reference_v<B>;

// [meta.trans.other]/3.5
template <class A, class B> requires meta_trans_other_3_5<A, B>
struct common_ref<A, B> {
  using type = cond_res<
    copy_cv_t<::std::remove_reference_t<A>, ::std::remove_reference_t<B>>&,
    copy_cv_t<::std::remove_reference_t<B>, ::std::remove_reference_t<A>>&
  >;
};
// [meta.trans.other]/3.6
template <class A, class B> requires meta_trans_other_3_6<A, B>
struct common_ref<A, B> : meta_trans_other_3_6_type<A, B> { };
// [meta.trans.other]/3.7
template <class A, class B> requires meta_trans_other_3_7<A, B>
struct common_ref<A, B> :
  common_ref<cref<A>, ::std::remove_reference_t<B>&>
{ };
// [meta.trans.other]/3.8
template <class A, class B> requires meta_trans_other_3_8<A, B>
struct common_ref<A, B> : common_ref<B, A> { };

} /* namespace apex::detail::prelude */

#endif /* not APEX_CHECK_API(concepts, 202002) */

namespace apex {

#if APEX_CHECK_API(concepts, 202002) and not APEX_USES_LIBSTDCXX
  using ::std::basic_common_reference;
  using ::std::common_reference_t;
  using ::std::common_reference;
#else
template <class, class, template <class> class, template <class> class>
struct basic_common_reference { };

template <class...> struct common_reference;
template <class... Ts>
using common_reference_t = typename common_reference<Ts...>::type;

// [meta.trans.other]/6.1
template <> struct common_reference<> { };
// [meta.trans.other]/6.2
template <class T> struct common_reference<T> { using type = T; };
// [meta.trans.other]/6.3

// [meta.trans.other]/6.3.1
template <class T, class U>
requires ::std::is_reference_v<T>
  and ::std::is_reference_v<U>
  and requires { typename detail::prelude::common_ref<T, U>::type; }
struct common_reference<T, U> :
  detail::prelude::common_ref<T, U>
{ };

// [meta.trans.other]/6.3.2
template <class T, class U>
requires requires {
  typename detail::prelude::xref<T>::type;
  typename basic_common_reference<
    remove_cvref_t<T>,
    remove_cvref_t<U>,
    detail::prelude::xref<T>::template type,
    detail::prelude::xref<U>::template type
  >::type;
} struct common_reference<T, U> : basic_common_reference<
  remove_cvref_t<T>,
  remove_cvref_t<U>,
  detail::prelude::xref<T>::template type,
  detail::prelude::xref<U>::template type
> { };
// [meta.trans.other]/6.3.3
template <class T, class U>
requires requires { typename detected<detail::prelude::cond_res, T, U>::type; }
struct common_reference<T, U> { using type = detail::prelude::cond_res<T, U>; };
// [meta.trans.other]/6.3.4
template <class T, class U>
requires requires { typename ::std::common_type<T, U>::type; }
struct common_reference<T, U> : ::std::common_type<T, U> { };

// [meta.trans.other]/6.4
template <class T, class U, class... Rest>
struct common_reference<T, U, Rest...> :
  common_reference<common_reference_t<T, U>, Rest...>
{ };

#endif /* APEX_CHECK_API(concepts, 202002) */
} /* namespace apex */

#endif /* APEX_DETAIL_PRELUDE_COMMON_REFERENCE_HPP */
