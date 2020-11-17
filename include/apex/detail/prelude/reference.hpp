#ifndef APEX_DETAIL_PRELUDE_REFERENCE_HPP
#define APEX_DETAIL_PRELUDE_REFERENCE_HPP

#include <type_traits>

namespace apex::detail::prelude {

template <class T> concept reference = ::std::is_reference_v<T>;
template <class T> concept lvalue_reference = reference<T> and ::std::is_lvalue_reference_v<T>;
template <class T> concept rvalue_reference = reference<T> and not lvalue_reference<T>;

} /* namespace apex::detail::prelude */

#endif /* APEX_DETAIL_PRELUDE_REFERENCE_HPP */
