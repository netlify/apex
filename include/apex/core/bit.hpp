#ifndef APEX_CORE_BIT_HPP
#define APEX_CORE_BIT_HPP

#include <apex/core/concepts.hpp>

#include <cstring>

#if __has_include(<bit>)
  #include <bit>
#endif /* __has_include(<bit>) */

namespace apex {
#if not APEX_CHECK_API(bit_cast, 201806)
namespace detail {

// TODO: turn this into a concept when we have time
// NOTE: clang provides __builtin_bit_cast, which can be used like
// return __builtin_bit_cast(To, value)
// But the basic requirements are still needed
/* This is the closest to "we can bit-cast these types" we can get under C++17 */
template <class From, class To>
concept bit_castable_from = requires {
  requires not (std::is_member_pointer_v<From> or std::is_member_pointer_v<To>);
  requires not (std::is_pointer_v<From> or std::is_pointer_v<To>);
  requires not (std::is_union_v<From> or std::is_union_v<To>);
  requires not (is_volatile_v<From> or is_volatile_v<To>);
  requires sizeof(From) == sizeof(To);
  requires trivially_copyable<From>;
  requires trivially_copyable<To>;
};
} /* namespace impl */
#endif /* not APEX_CHECK_API(bit_cast, 201806) */

#if APEX_CHECK_API(endian, 201907)
using ::std::endian;
#else
enum class endian {
#if APEX_SYSTEM_WINDOWS
  little = 0,
  big = 1,
  native = little,
#else
  little = __ORDER_LITTLE_ENDIAN__,
  big = __ORDER_BIG_ENDIAN__,
  native = __BYTE_ORDER__,
#endif /* APEX_SYSTEM_WINDOWS */
};
#endif /* APEX_CHECK_API(endian, 201907) */

[[nodiscard]] constexpr int popcount (unsigned_integral auto v) noexcept {
  using type = decltype(v);
  if constexpr (same_as<type, unsigned long long>) { return __builtin_popcountll(v); }
  else if constexpr (same_as<type, unsigned long>) { return __builtin_popcountl(v); }
  else { return __builtin_popcount(v); }
}

// FIXME: This doesn't handle the case of unsigned long long vs unsigned long being
// defined for uint64_t/u64 within a codebase. We need to still handle them.
constexpr auto byteswap (integral auto v) noexcept {
  constexpr auto digits = ::std::numeric_limits<decltype(v)>::digits;
  using type = ::std::make_unsigned_t<decltype(v)>;
  if constexpr (signed_integral<decltype(v)>) { return byteswap(bit_cast<type>(v)); }
  else if constexpr (digits == 64) { return __builtin_bswap64(v); }
  else if constexpr (digits == 32) { return __builtin_bswap32(v); }
  else if constexpr (digits == 16) { return __builtin_bswap16(v); }
  else { return v; }
}

// TODO: Support signed byteswaps
constexpr auto byteswap  (unsigned_integral auto v) noexcept {
  using type = decltype(v);
  if constexpr (same_as<u64, type>) { return __builtin_bswap64(v); }
  else if constexpr (same_as<u32, type>) { return __builtin_bswap32(v); }
  else if constexpr (same_as<u16, type>) { return __builtin_bswap16(v); }
  else { return v; }
}

// TODO: Support signed integers and non-64/32 bit integers?
template <class T>
[[nodiscard]] constexpr T rotr (T x, int s) noexcept {
  constexpr auto digits = ::std::numeric_limits<T>::digits;
  static_assert(digits == 64 or digits == 32);
  return (x >> s) | (x << (digits - s));
}

// TODO: Bring up to proper std support
template <class T>
[[nodiscard]] constexpr T rotl (T x, int s) noexcept {
  constexpr auto digits = ::std::numeric_limits<T>::digits;
  static_assert(digits == 64 or digits == 32);
  return (x << s) | (x >> (digits - s));
}

// TODO: Bring up to proper std support
constexpr int countl_zero (unsigned_integral auto x) noexcept {
  using type = remove_cvref_t<decltype(x)>;
  if constexpr (same_as<unsigned long long, type>) { return __builtin_clzll(x); }
  else if constexpr (std::is_same_v<unsigned long, type>) { return __builtin_clzl(x); }
  else { return __builtin_clz(x); }
}

constexpr int countr_zero (unsigned_integral auto x) noexcept {
  using type = remove_cvref_t<decltype(x)>;
  if constexpr (same_as<unsigned long long, type>) { return __builtin_ctzll(x); }
  else if constexpr (same_as<unsigned long, type>) { return __builtin_ctzl(x); }
  else { return __builtin_ctz(x); }
}

constexpr int countl_one (unsigned_integral auto x) noexcept { return countl_zero(~x); }
constexpr int countr_one (unsigned_integral auto x) noexcept { return countr_zero(~x); }

#if APEX_CHECK_API(bit_cast, 201806)
using ::std::bit_cast;
#else
template <class To>
constexpr auto bit_cast (detail::bit_castable_from<To> auto const& from) noexcept {
#if APEX_CHECK_BUILTIN(bit_cast)
  return __builtin_bit_cast(To, from);
#else
  std::aligned_storage_t<sizeof(To), alignof(To)> storage;
  return *static_cast<To*>(std::memcpy(&storage, &from, sizeof(To)));
#endif
}
#endif /* APEX_CHECK_API(bit_cast, 201806) */

// Not part of the standard, but still useful!
[[noreturn]] void unreachable () noexcept { __builtin_unreachable(); }

} /* namespace apex */

#endif /* APEX_CORE_BIT_HPP */
