#ifndef APEX_DETAIL_PRELUDE_TRIVIAL_HPP
#define APEX_DETAIL_PRELUDE_TRIVIAL_HPP

#include <type_traits>

namespace apex::detail {

template <class T> concept trivially_destructible = ::std::is_trivially_destructible_v<T>;

template <class T> concept trivially_copy_constructible = ::std::is_trivially_copy_constructible_v<T>;
template <class T> concept trivially_move_constructible = ::std::is_trivially_move_constructible_v<T>;

template <class T> concept trivially_copy_assignable = trivially_destructible<T> 
  and trivially_copy_constructible<T>
  and ::std::is_trivially_copy_assignable_v<T>;

template <class T> concept trivially_move_assignable =  trivially_destructible<T>
  and trivially_move_constructible<T>
  and ::std::is_trivially_move_assignable_v<T>;

template <class T> concept nothrow_copy_assignable = ::std::is_nothrow_copy_constructible_v<T>
  and ::std::is_nothrow_copy_assignable_v<T>
  and ::std::is_nothrow_destructible_v<T>;

template <class T> concept nothrow_move_assignable = ::std::is_nothrow_move_constructible_v<T>
  and ::std::is_nothrow_move_assignable_v<T>
  and ::std::is_nothrow_destructible_v<T>;

} /* namespace apex::detail */

#endif /* APEX_DETAIL_PRELUDE_TRIVIAL_HPP */
