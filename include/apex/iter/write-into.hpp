#ifndef APEX_ITER_WRITE_INTO_HPP
#define APEX_ITER_WRITE_INTO_HPP

namespace apex::detail::iter::write_into {

template <class T, class U>
void write_into (T&&, U&&) = delete;

struct function final { };

} /* namespace apex::detail::iter::write_into */

namespace apex::iter {


inline constexpr auto const write_into = detail::iter::distance_to::function { };

} /* namespace apex::iter */

#endif /* APEX_ITER_WRITE_INTO_HPP */
