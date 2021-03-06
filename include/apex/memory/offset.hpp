#ifndef APEX_MEMORY_OFFSET_HPP
#define APEX_MEMORY_OFFSET_HPP

#include <apex/core/concepts.hpp>

// offset_ptr is useful for creating zero allocation data structures within
// a block of memory. The offset type MUST be an integral type. If it is
// unsigned, it CANNOT be decremented.
// Additionally, we need to specialize std::pointer_traits for this type,
// as the difference_type is whatever we use as the offset_type

namespace apex {
inline namespace v1 {

// TODO: specialize for offset_ptr<T[N]>
template <class T, integral Offset>
struct offset_ptr {
  using offset_type = Offset;

  using difference_type = offset_type;
  using pointer = std::add_pointer_t<T>;

  offset_ptr (offset_ptr const&) noexcept = default;
  offset_ptr () noexcept = default;

  offset_ptr (nullptr_t) noexcept;
  offset_ptr (pointer) noexcept;

  offset_ptr& operator = (offset_ptr const&) = default;

  void swap (offset_ptr&) noexcept;

  explicit operator bool () const noexcept;

  decltype(auto) operator * () const noexcept;
  pointer operator -> () const noexcept;
  pointer get () const noexcept;

private:
  offset_type offset { };
};

}} /* namespace apex::v1 */

#endif /* APEX_MEMORY_OFFSET_HPP */
