#ifndef APEX_DETAIL_PRELUDE_CONVERTIBLE_TO_HPP
#define APEX_DETAIL_PRELUDE_CONVERTIBLE_TO_HPP

#include <type_traits>

namespace apex {

template <class From, class To>
concept convertible_to = ::std::is_convertible_v<From, To>
  and requires (::std::add_rvalue_reference_t<From> (&f)()) {
    static_cast<To>(f());
  };

} /* namespace apex */

#endif /* APEX_DETAIL_PRELUDE_CONVERTIBLE_TO_HPP */