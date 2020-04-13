#ifndef APEX_UTILITY_HPP
#define APEX_UTILITY_HPP

#include <utility>

#include <apex/traits.hpp>

namespace apex {
inline namespace v1 {

using std::declval;

template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

template <class T>
constexpr auto to_underlying (T value) noexcept -> typename std::enable_if<
  std::is_enum_v<T>
> { return static_cast<std::underlying_type_t<T>>(value); }

// NOTE: The paper this is for has undergone some feedback, including a new
// name that the author wasn't present for and couldn't defend. We're sticking
// with offset_to. Additionally, it wasn't given a specific header/module
// location. For this reason, it's placed inside the <apex/utility.hpp> header
/* This is relies on implementation specific behavior.
 * We assume Windows/macOS/Linux ABIs
 */
template <
  class T,
  class=typename std::enable_if<
    std::conjunction_v<
      std::is_member_object_pointer<T>,
      std::is_standard_layout<class_of_t<T>>
    >
  >::type
> ptrdiff_t offset_to (T const& member) noexcept {
  constexpr bool valid = sizeof(T) == sizeof(ptrdiff_t) or
    sizeof(T) == sizeof(i32);
  static_assert(valid, "Cannot safely get offset of member pointer");
  using value_type = std::conditional_t<
    sizeof(T) == sizeof(ptrdiff_t),
    ptrdiff_t,
    i32
  >;
  value_type value { };
  std::memcpy(&value, &member, sizeof(value));
  return value;
}


}} /* namespace apex::v1 */

#endif /* APEX_UTILITY_HPP */
