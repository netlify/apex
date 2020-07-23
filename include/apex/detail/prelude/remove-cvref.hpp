#ifndef APEX_DETAIL_PRELUDE_REMOVE_REFERENCE_HPP
#define APEX_DETAIL_PRELUDE_REMOVE_REFERENCE_HPP

#include <apex/detail/prelude/macros.hpp>
#include <type_traits>

namespace apex {

#if APEX_CHECK_API(remove_cvref, 201711)
  APEX_WARN("std::remove_cvref is available under this implementation")
  using ::std::remove_cvref_t;
  using ::std::remove_cvref;
#else
  template <class T>
  struct remove_cvref :
    ::std::remove_cv<::std::remove_reference_t<T>>
  { };
  template <class T>
  using remove_cvref_t = typename remove_cvref<T>::type;
#endif /* APEX_CHECK_API(remove_cvref, 201711) */

} /* namespace apex */

#endif /* APEX_DETAIL_PRELUDE_REMOVE_REFERENCE_HPP */