#ifndef APEX_MEMORY_STD_HPP
#define APEX_MEMORY_STD_HPP

#include <apex/core/concepts.hpp>
#include <memory>

// This provides shims for various std:: functions

namespace apex {
inline namespace v1 {

#if __cpp_lib_to_address >= 201711L
using ::std::to_address;
#else
template <class T>
constexpr auto to_address (T const& p) noexcept { return p.operator->(); }

template <class T>
constexpr T* to_address (T* p) noexcept {
  static_assert(not std::is_function_v<T>);
  return p;
}
#endif /* __cpp_lib_to_address >= 201711L */

#if __cpp_lib_constexpr_dynamic_alloc >= 201907L
using ::std::construct_at;
using ::std::destroy_at;
#else
// Neither of these can be constexpr under C++17 for a variety of reasons.
// However, we can't use them in a constexpr context under C++17 ANYHOW, so
// it doesn't really matter
template <class T>
void destroy_at (T* ptr) noexcept(std::is_nothrow_destructible_v<T>) {
  if constexpr (std::is_array_v<T>) {
    for (auto& element : *ptr) {
      apex::destroy_at(std::addressof(element));
    }
  } else if constexpr (not std::is_trivially_destructible_v<T>) {
    ptr->~T();
  }
}

template <class T, class... Args>
requires constructible_from<T, Args...>
auto construct_at (T* location, Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>) {
  if constexpr (std::is_aggregate_v<T>) {
    return ::new (static_cast<void*>(location)) T { std::forward<Args>(args)... };
  } else {
    return ::new (static_cast<void*>(location)) T(std::forward<Args>(args)...);
  }
}
#endif /* __cpp_lib_constexpr_dynamic_alloc >= 201907L */

}} /* namespace apex::v1 */

#endif /* APEX_MEMORY_STD_HPP */
