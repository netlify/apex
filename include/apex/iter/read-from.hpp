#ifndef APEX_ITER_READ_FROM_HPP
#define APEX_ITER_READ_FROM_HPP

namespace apex::detail::iter::read_from {

struct function final { };

} /* namespace apex::detail::iter::read_from */

namespace apex::iter {

inline constexpr auto const read_from = detail::iter::read_from::function { };

} /* namespace apex::iter */

#endif /* APEX_ITER_READ_FROM_HPP */
