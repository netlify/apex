#ifndef APEX_ITER_EQUAL_TO_HPP
#define APEX_ITER_EQUAL_TO_HPP

namespace apex::detail::iter::equal_to {

struct function final { };

} /* namespace apex::detail::iter::equal_to */

namespace apex::iter {

inline constexpr auto const equal_to = detail::iter::equal_to::function { };

} /* namespace apex::iter */

#endif /* APEX_ITER_EQUAL_TO_HPP */
