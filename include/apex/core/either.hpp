#ifndef APEX_CORE_EITHER_HPP
#define APEX_CORE_EITHER_HPP

#include <apex/detail/core/either.hpp>
#include <apex/core/exception.hpp>

namespace apex {

template <class A, class B>
struct either final : private detail::either::base<A, B> {
  using value_type = A;
  using other_type = B;
  using base_type = detail::either::base<A, B>;

  using base_type::assume_value;
  using base_type::assume_other;
  using base_type::has_value;
  using base_type::has_other;
  using base_type::index;
  using base_type::base_type;

  // using base_type::try_emplace
  // using base_type::emplace;

  constexpr either (either const&) noexcept(safely_copy_constructible<base_type>) = default;
  constexpr either (either&&) noexcept(safely_move_constructible<base_type>) = default;
  constexpr either () = delete;

  constexpr either& operator = (either const&) noexcept(safely_copy_assignable<base_type>) = default;
  constexpr either& operator = (either&&) noexcept(safely_move_assignable<base_type>) = default;

  template <class... Args> requires constructible_from<value_type, Args...>
  [[nodiscard]] value_type& try_emplace (::std::in_place_index_t<0>, Args&&... args)
  noexcept(safely_constructible_from<value_type, Args...>) {
    if (this->has_value()) { return this->assume_value(); }
    base_type::construct_value(static_cast<Args&&>(args)...);
    return this->assume_value();
  }

  template <class... Args> requires constructible_from<other_type, Args...>
  [[nodiscard]] other_type& try_emplace (::std::in_place_index_t<1>, Args&&... args)
  noexcept(safely_constructible_from<other_type, Args...>) {
    if (this->has_other()) { return this->assume_other(); }
    base_type::construct_other(static_cast<Args&&>(args)...);
    return this->assume_other();
  }

  constexpr value_type const&& value () const&& noexcept(false) {
    if (not this->has_value()) {
      throw bad_either_access { static_cast<other_type const&&>(this->assume_other()) };
    }
    return this->assume_value();
  }

  constexpr value_type const& value () const& noexcept(false) {
    if (not this->has_value()) {
      throw bad_either_access { this->assume_other() };
    }
    return this->assume_value();
  }

  constexpr value_type&& value () && noexcept(false) {
    if (not this->has_value()) {
      throw bad_either_access { static_cast<other_type&&>(this->assume_other()) };
    }
    return this->assume_value();
  }

  constexpr value_type& value () & noexcept(false) {
    if (not this->has_value()) {
      throw bad_either_access { this->assume_other() };
    }
    return this->assume_value();
  }

  constexpr other_type const&& other () const&& noexcept(false) {
    if (not this->has_other()) {
      throw bad_either_access { static_cast<value_type const&&>(this->assume_value()) };
    }
    return this->assume_other();
  }

  constexpr other_type const& other () const& noexcept(false) {
    if (not this->has_other()) {
      throw bad_either_access { this->assume_value() };
    }
    return this->assume_other();
  }

  constexpr other_type&& other () && noexcept(false) {
    if (not this->has_other()) {
      throw bad_either_access { this->assume_value() };
    }
    return this->assume_other();
  }

  constexpr other_type& other () & noexcept(false) {
    if (not this->has_other()) {
      throw bad_either_access { this->assume_value() };
    }
    return this->assume_other();
  }

  template <convertible_to<value_type> U> requires copy_constructible<value_type>
  constexpr value_type value_or (U&& default_value) const&
  noexcept(safely_copy_constructible<value_type>) {
    if (this->has_value()) { return this->assume_value(); }
    return static_cast<value_type>(static_cast<U&&>(default_value));
  }

  template <convertible_to<value_type> U> requires move_constructible<value_type>
  constexpr value_type value_or (U&& default_value) &&
  noexcept(safely_move_constructible<value_type>) {
    if (this->has_value()) { return static_cast<value_type&&>(this->assume_value()); }
    return static_cast<value_type>(static_cast<U&&>(default_value));
  }

  template <convertible_to<other_type> U> requires copy_constructible<other_type>
  constexpr other_type other_or (U&& default_value) const&
  noexcept(safely_copy_constructible<other_type>) {
    if (this->has_other()) { return this->assume_other(); }
    return static_cast<other_type>(static_cast<U&&>(default_value));
  }

  template <convertible_to<other_type> U> requires move_constructible<other_type>
  constexpr other_type other_or (U&& default_value) &&
  noexcept(safely_move_constructible<other_type>) {
    if (this->has_other()) { return static_cast<other_type&&>(this->assume_other()); }
    return static_cast<other_type>(static_cast<U&&>(default_value));
  }
};

template <class A, class B>
constexpr bool operator == (either<A, B> const& lhs, either<A, B> const& rhs)
noexcept(noexcept(lhs.assume_value() == rhs.assume_value())
  and noexcept(lhs.assume_other() == rhs.assume_other())) {
  if (lhs.has_value() and rhs.has_value()) { return lhs.assume_value() == rhs.assume_value(); }
  if (lhs.has_other() and rhs.has_other()) { return lhs.assume_other() == rhs.assume_other(); }
  return false;
}

template <class A, class B>
constexpr bool operator != (either<A, B> const& lhs, either<A, B> const& rhs)
noexcept(noexcept(not (lhs == rhs))) {
  return not (lhs == rhs);
}

//template <class A, class B, class C, class D>
//constexpr bool operator == (either<A, B> const& lhs, either<C, D> const& rhs)
//noexcept(noexcept(lhs.assume_value() == rhs.assume_value())
//    and noexcept(lhs.assume_other() == rhs.assume_other())) {
// }
//
//template <class A, class B, class C, class D>
//constexpr bool operator != (either<A, B> const& lhs, either<C, D> const& rhs)
//noexcept(noexcept(not (lhs == rhs))) {
//  return not (lhs == rhs);
//}

} /* namespace apex */

#endif /* APEX_CORE_EITHER_HPP */
