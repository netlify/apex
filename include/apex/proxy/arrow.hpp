#ifndef APEX_PROXY_ARROW_HPP
#define APEX_PROXY_ARROW_HPP

#include <apex/core/concepts.hpp>

// Used as a proxy for non-pointer returned operator ->() calls
// https://quuxplusone.github.io/blog/2019/02/06/arrow-proxy
// Most useful currently for apex::mixin::iterator/iterable

namespace apex::proxy {

template <class T>
struct arrow final {
  using value_type = T;

  using const_pointer = add_pointer_t<add_const_t<remove_reference_t<value_type>>>;
  using pointer = add_pointer_t<remove_reference_t<value_type>>;

  template <constructible_from<value_type> U>
  constexpr arrow (U&& value) noexcept(std::is_nothrow_constructible_v<value_type, U>) :
    value { std::forward<U>(value) }
  { }

  constexpr const_pointer operator -> () const noexcept { return std::addressof(this->value); }
  constexpr pointer operator -> () noexcept { return std::addressof(this->value); }

private:
  value_type value;
};

template <class T> arrow (T&&) -> arrow<T>;

} /* namespace apex::proxy */

#endif /* APEX_PROXY_ARROW_HPP */
