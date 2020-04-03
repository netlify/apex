#ifndef APEX_FUNCTIONAL_HPP
#define APEX_FUNCTIONAL_HPP

namespace apex {
inline namespace v1 {

template <class F, class... Args>
constexpr auto bind_front (F&& f, Args&&... args) { }

}} /* namespace apex::v1 */

#endif /* APEX_FUNCTIONAL_HPP */
