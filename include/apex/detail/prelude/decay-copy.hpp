#ifndef APEX_DETAIL_PRELUDE_DECAY_COPY_HPP
#define APEX_DETAIL_PRELUDE_DECAY_COPY_HPP

#include <type_traits>

namespace apex::detail::prelude {

template <class T>
constexpr ::std::decay_t<T> decay_copy (T&& value)
noexcept(::std::is_nothrow_convertible_v<T, ::std::decay_t<T>>) {
  return static_cast<T&&>(value);
}

} /* namespace apex::detail::prelude */

#endif /* APEX_DETAIL_PRELUDE_DECAY_COPY_HPP */
