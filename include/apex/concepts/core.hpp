#ifndef APEX_CONCEPTS_CORE_HPP
#define APEX_CONCEPTS_CORE_HPP

#include <apex/core/traits.hpp>

namespace apex::concepts {

// sadly, we can't enforce subsumption like C++20 requires :/
template <class T, class U>
inline constexpr bool same_as = conjunction_v<
  std::is_same<T, U>,
  std::is_same<U, T>
>;

template <class Derived, class Base>
inline constexpr bool derived_from = conjunction_v<
  std::is_base_of<Base, Derived>,
  std::is_convertible<Derived const volatile*, Base const volatile*>
>;

template <class T>
inline constexpr bool integral = std::is_integral_v<T>;

template <class T>
inline constexpr bool signed_integral = conjunction_v<
  std::is_integral<T>,
  std::is_signed<T>
>;

template <class T>
inline constexpr bool unsigned_integral = conjunction_v<
  std::is_integral<T>,
  std::is_unsigned<T>
>;

} /* namespace apex::concepts */

#endif /* APEX_CONCEPTS_CORE_HPP */
