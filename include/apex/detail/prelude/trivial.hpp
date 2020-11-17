#ifndef APEX_DETAIL_PRELUDE_TRIVIAL_HPP
#define APEX_DETAIL_PRELUDE_TRIVIAL_HPP

#include <apex/detail/prelude/reference.hpp>

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

template <class T> concept trivially_storable = trivially_destructible<T>
  or prelude::lvalue_reference<T>;

} /* namespace apex::detail */

#endif /* APEX_DETAIL_PRELUDE_TRIVIAL_HPP */
