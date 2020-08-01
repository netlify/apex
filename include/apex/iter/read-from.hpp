#ifndef APEX_ITER_READ_FROM_HPP
#define APEX_ITER_READ_FROM_HPP

namespace apex::detail::iter::read_from {

struct function final { };

} /* namespace apex::detail::iter::read_from */

namespace apex::iter {

/** @brief Read a value from the given parameter.
 * Member versions of `read_from` are called first, followed by ADL lookups
 * of `read_from`.
 * @note `read_from` is free to return references *or* values.
 * @ingroup cpo-iterable
 */
inline constexpr auto const read_from = detail::iter::read_from::function { };

} /* namespace apex::iter */

#endif /* APEX_ITER_READ_FROM_HPP */
