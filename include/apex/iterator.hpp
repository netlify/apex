#ifndef APEX_ITERATOR_HPP
#define APEX_ITERATOR_HPP

namespace apex {
inline namespace v1 {

template <
  class C,
  class R=std::common_type_t<
    std::ptrdiff_t,
    std::make_signed_t<decltype(std::declval<C>().size()>
  >
> constexpr auto ssize (C const& c) { return static_cast<R>(c.size()); }

}} /* namespace apex::v1 */

#endif /* APEX_ITERATOR_HPP */
