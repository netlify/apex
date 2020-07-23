#ifndef APEX_DETAIL_PRELUDE_TYPES_HPP
#define APEX_DETAIL_PRELUDE_TYPES_HPP

#include <cstddef>
#include <cstdint>

/* Using a large amount of nested namespaces should discourage folks using it
 * 😉
 */
namespace apex::detail::prelude::types {
/**
 * Custom implementation of `std::ignore`, to reduce the need to include
 * <tuple>
 */
struct ignore_t final {
  template <class T>
  constexpr ignore_t const& operator = (T&&) const noexcept { return *this; }
};

} /* namespace apex::detail */

namespace apex {

using ::std::nullptr_t;
using ::std::ptrdiff_t;
using ::std::size_t;
using ::std::byte;

using i64 = ::std::int64_t;
using i32 = ::std::int32_t;
using i16 = ::std::int16_t;
using i8 = ::std::int8_t;

using u64 = ::std::uint64_t;
using u32 = ::std::uint32_t;
using u16 = ::std::uint16_t;
using u8 = ::std::uint8_t;

using f64 = double;
using f32 = float;
/* This is a clang extension.
 * When working with this type, it first is converted to a f32, and then
 * converted back down to an f16. This behavior cannot currently be changed
 * under clang. It is stored in the binary16 format.
 */
using f16 = __fp16;

/* Turns out, having an empty struct is really useful! */

struct empty { };
/* Used for prioritizing possibly ambiguous overloads. I is the "level", where
 * higher numbers mean a higher level of preference
 */
template <size_t I> struct preference : preference<I - 1> { };
template <> struct preference<0> { };
template <size_t I> inline constexpr auto const prefer = preference<I> { };

[[clang::no_destroy]] inline constexpr auto const ignore = detail::prelude::types::ignore_t { };

} /* namespace apex */

#endif /* APEX_DETAIL_PRELUDE_TYPES_HPP */
