#ifndef APEX_TYPES_HPP
#define APEX_TYPES_HPP

#include <cstddef>
#include <cstdint>

inline namespace apex {
inline namespace v1 {

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

}} /* namespace apex::v1 */

#endif /* APEX_TYPES_HPP */
