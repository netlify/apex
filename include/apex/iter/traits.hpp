#ifndef APEX_ITER_TRAITS_HPP
#define APEX_ITER_TRAITS_HPP

#include <apex/core/prelude.hpp>

namespace apex::iter::detail {

template <class T> using sentinel_type = typename T::sentinel_type;

} /* namespace apex::iter::detail */

namespace apex::iter {

template <class I>
struct sentinel_of : detected_or<I, detail::sentinel_type, I> { };

template <class I>
using sentinel_of_t = typename sentinel_of<I>::type;


} /* namespace apex::iter */

#endif /* APEX_ITER_TRAITS_HPP */
