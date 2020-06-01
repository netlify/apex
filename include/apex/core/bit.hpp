#ifndef APEX_CORE_BIT_HPP
#define APEX_CORE_BIT_HPP

#include <apex/core/concepts.hpp>

#include <cstring>

#if __has_include(<bit>)
  #include <bit>
#endif /* __has_include(<bit>) */

namespace apex {
#if not APEX_CHECK_API(bit_cast, 201806)
namespace impl {

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
};
template <class To, class From>
constexpr bool can_bit_cast = std::conjunction_v<
  std::negation<std::disjunction<std::is_member_pointer<From>, std::is_member_pointer<To>>>,
  std::negation<std::disjunction<std::is_volatile<From>, std::is_volatile<To>>>,
  std::negation<std::disjunction<std::is_pointer<From>, std::is_pointer<To>>>,
  std::negation<std::disjunction<std::is_union<From>, std::is_union<To>>>
>;

} /* namespace impl */
#endif /* not APEX_CHECK_API(bit_cast, 201806) */

#if APEX_CHECK_API(endian, 201907)
using std::endian;
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

// TODO: just make these overloads or something similar
template <unsigned_integral T>
[[nodiscard]] constexpr int popcount (T v) noexcept {
  if constexpr (std::is_same_v<T, unsigned long long>) { return __builtin_popcountll(v); }
  else if constexpr (std::is_same_v<T, unsigned long>) { return __builtin_popcountl(v); }
  else { return __builtin_popcount(v); }
}

// TODO: Support signed byteswaps
template <unsigned_integral T>
constexpr T byteswap  (T v) noexcept {
  if constexpr (std::is_same_v<u64, T>) { return __builtin_bswap64(v); }
  else if constexpr (std::is_same_v<u32, T>) { return __builtin_bswap32(v); }
  else if constexpr (std::is_same_v<u16, T>) { return __builtin_bswap16(v); }
  else { return v; }
}

// TODO: Support signed integers and non-64/32 bit integers?
template <class T>
[[nodiscard]] constexpr T rotr (T x, int s) noexcept {
  constexpr auto digits = std::numeric_limits<T>::digits;
  static_assert(digits == 64 or digits == 32);
  return (x >> s) | (x << (digits - s));
}

// TODO: Bring up to proper std support
template <class T>
[[nodiscard]] constexpr T rotl (T x, int s) noexcept {
  constexpr auto digits = std::numeric_limits<T>::digits;
  static_assert(digits == 64 or digits == 32);
  return (x << s) | (x >> (digits - s));
}

// TODO: Bring up to proper std support
template <unsigned_integral T>
constexpr int countl_zero (T x) {
  if constexpr (std::is_same_v<unsigned long long, T>) { return __builtin_clzll(x); }
  else if constexpr (std::is_same_v<unsigned long, T>) { return __builtin_clzl(x); }
  else { return __builtin_clz(x); }
}

template <unsigned_integral T>
constexpr int countr_zero (T x) {
  if constexpr (std::is_same_v<unsigned long long, T>) { return __builtin_ctzll(x); }
  else if constexpr (std::is_same_v<unsigned long, T>) { return __builtin_ctzl(x); }
  else { return __builtin_ctz(x); }
}

template <unsigned_integral T>
constexpr int countl_one (T x) { return countl_zero(~x); }

template <unsigned_integral T>
constexpr int countr_one (T x) { return countr_zero(~x); }

#if APEX_CHECK_API(bit_cast, 201806)
using ::std::bit_cast;
#else
template <class To, class From>
requires impl::bit_castable_from<From, To>
  and (sizeof(To) == sizeof(From))
  and std::is_trivially_copyable_v<From>
  and std::is_trivially_copyable_v<To>
constexpr auto bit_cast (From const& from) noexcept -> To {
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
