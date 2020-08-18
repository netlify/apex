#ifndef APEX_DETAIL_PRELUDE_XVALUE_OF_HPP
#define APEX_DETAIL_PRELUDE_XVALUE_OF_HPP

#include <type_traits>

namespace apex {

template <class T>
using xvalue_of = typename ::std::remove_reference<T>::type&&;

} /* namespace apex */

#endif /* APEX_DETAIL_PRELUDE_XVALUE_OF_HPP */
