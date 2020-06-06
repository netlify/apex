#ifndef APEX_CORE_TYPES_HPP
#define APEX_CORE_TYPES_HPP

#include <cstddef>
#include <cstdint>

namespace apex {

using std::nullptr_t;
using std::ptrdiff_t;
using std::size_t;
using std::byte;

using czstr = char const*;
using zstr = char*;

using i64 = std::int64_t;
using i32 = std::int32_t;
using i16 = std::int16_t;
using i8 = std::int8_t;

using u64 = std::uint64_t;
using u32 = std::uint32_t;
using u16 = std::uint16_t;
using u8 = std::uint8_t;

using f64 = double;
using f32 = float;

struct empty { };

// implementation of std::ignore, but placed into a header that *doesn't*
// require including <tuple>
struct ignore_t final {
  template <class T>
  constexpr ignore_t const& operator = (T&&) const noexcept { return *this; }
};

inline constexpr auto ignore = ignore_t { };

} /* namespace apex */

#endif /* APEX_CORE_TYPES_HPP */
