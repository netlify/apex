#ifndef APEX_CORE_EITHER_HPP
#define APEX_CORE_EITHER_HPP

#include <apex/detail/core/either.hpp>
#include <apex/core/exception.hpp>

namespace apex {

/* TODO: Add "std::get" and "std::get_if" support */
/* TODO: Add "holds_alternative()" support */
/* TODO: Add "valueless_by_exception()" support */

/* TODO: come up with better name, put into plain `apex` namespace */
using detail::prelude::lvalue_reference;

template <class A, class B>
struct either final : private detail::either::base<A, B> {
  using value_type = A;
  using other_type = B;
  using base_type = detail::either::base<A, B>;

  template <class... Args>
  requires constructible_from<value_type, Args...> and (not lvalue_reference<value_type>)
  constexpr explicit(not sizeof...(Args)) either (::std::in_place_index_t<0>, Args&&... args)
  noexcept(safely_constructible_from<value_type, Args...>) :
    base_type(detail::either::value, static_cast<Args&&>(args)...)
  { }

  template <class... Args>
  requires constructible_from<other_type, Args...> and (not lvalue_reference<other_type>)
  constexpr explicit(not sizeof...(Args)) either (::std::in_place_index_t<1>, Args&&... args)
  noexcept(safely_constructible_from<other_type, Args...>) :
    base_type(detail::either::other, static_cast<Args&&>(args)...)
  { }

  template <class T> requires lvalue_reference<value_type> and convertible_to<T&, value_type>
  constexpr either (::std::in_place_index_t<0>, T& value) noexcept :
    base_type(detail::either::value, ::std::addressof(value))
  { }

  template <class T> requires lvalue_reference<other_type> and convertible_to<T&, other_type>
  constexpr either (::std::in_place_index_t<1>, T& value) noexcept :
    base_type(detail::either::other, ::std::addressof(value))
  { }

  template <class T> requires lvalue_reference<value_type> and convertible_to<T&, value_type>
  constexpr either (::std::in_place_index_t<0>, ::std::reference_wrapper<T> value) noexcept :
    either(::std::in_place_index<0>, value.get())
  { }

  template <class T> requires lvalue_reference<other_type> and convertible_to<T&, other_type>
  constexpr either (::std::in_place_index_t<1>, ::std::reference_wrapper<T> value) noexcept :
    either(::std::in_place_index<1>, value.get())
  { }

  template <class T, class... Args>
  requires constructible_from<either, ::std::in_place_index_t<int(same_as<T, other_type>)>, Args...>
    and distinct_from<value_type, other_type>
    and (same_as<T, value_type> or same_as<T, other_type>)
  constexpr explicit(not sizeof...(Args)) either (::std::in_place_type_t<T>, Args&&... args)
  noexcept(safely_constructible_from<either, ::std::in_place_index_t<int(same_as<T, other_type>)>, Args...>) :
    either(::std::in_place_index<int(same_as<T, other_type>)>, static_cast<Args&&>(args)...)
  { }

  using base_type::assume_value;
  using base_type::assume_other;
  using base_type::has_value;
  using base_type::has_other;
  using base_type::index;

  constexpr either (either const&) noexcept(safely_copy_constructible<base_type>) = default;
  constexpr either (either&&) noexcept(safely_move_constructible<base_type>) = default;
  constexpr either () = delete;

  constexpr either& operator = (either const&) noexcept(safely_copy_assignable<base_type>) = default;
  constexpr either& operator = (either&&) noexcept(safely_move_assignable<base_type>) = default;

  constexpr decltype(auto) value () const&& noexcept(false) {
    if (this->has_value()) { return static_cast<either const&&>(*this).assume_value(); }
    throw bad_either_access(static_cast<either const&&>(*this).assume_other());
  }

  constexpr decltype(auto) value () const& noexcept(false) {
    if (this->has_value()) { return this->assume_value(); }
    throw bad_either_access(this->assume_other());
  }

  constexpr decltype(auto) value () && noexcept(false) {
    if (this->has_value()) { return static_cast<either&&>(*this).assume_value(); }
    throw bad_either_access(static_cast<other_type&&>(this->assume_other()));
  }

  constexpr decltype(auto) value () & noexcept(false) {
    if (this->has_value()) { return this->assume_value(); }
    throw bad_either_access(this->assume_other());
  }

  constexpr decltype(auto) other () const&& noexcept(false) {
    if (this->has_other()) { return static_cast<either const&&>(*this).assume_other(); }
    throw bad_either_access(static_cast<either const&&>(*this).assume_value());
  }

  constexpr decltype(auto) other () const& noexcept(false) {
    if (this->has_other()) { return this->assume_other(); }
    throw bad_either_access(this->assume_value());
  }

  constexpr decltype(auto) other () && noexcept(false) {
    if (this->has_other()) { return static_cast<either&&>(*this).assume_other(); }
    throw bad_either_access(static_cast<either&&>(*this).assume_value());
  }

  constexpr decltype(auto) other () & noexcept(false) {
    if (this->has_other()) { return this->assume_other(); }
    throw bad_either_access(this->assume_value());
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

  template <class... Args> requires constructible_from<value_type, Args...>
  [[nodiscard]] value_type& try_emplace (::std::in_place_index_t<0>, Args&&... args)
  noexcept(safely_constructible_from<value_type, Args...>) {
    if (this->has_value()) { return this->assume_value(); }
    return this->emplace(::std::in_place_index<0>, static_cast<Args&&>(args)...);
  }

  template <class... Args> requires constructible_from<other_type, Args...>
  [[nodiscard]] other_type& try_emplace (::std::in_place_index_t<1>, Args&&... args)
  noexcept(safely_constructible_from<other_type, Args...>) {
    if (this->has_other()) { return this->assume_other(); }
    return this->emplace(::std::in_place_index<1>, static_cast<Args&&>(args)...);
  }

  /* TODO: Require T not be an lvalue_reference */
  template <class T, class... Args>
  requires constructible_from<T, Args...>
    and distinct_from<value_type, other_type>
    and (same_as<T, value_type> or same_as<T, other_type>)
    and (not lvalue_reference<T>)
  [[nodiscard]] value_type& try_emplace (::std::in_place_type_t<T>, Args&&... args)
  noexcept(safely_constructible_from<T, Args...>) {
    constexpr int index = same_as<T, other_type>;
    return this->try_emplace(::std::in_place_index<index>, static_cast<Args&&>(args)...);
  }

  template <class... Args> requires constructible_from<value_type, Args...>
  [[nodiscard]] value_type& emplace (::std::in_place_index_t<0>, Args&&... args)
  noexcept(safely_constructible_from<value_type, Args...>) {
    this->clear();
    base_type::construct_value(static_cast<Args&&>(args)...);
    return this->assume_value();
  }

  template <class... Args> requires constructible_from<other_type, Args...>
  [[nodiscard]] other_type& emplace (::std::in_place_index_t<1>, Args&&... args)
  noexcept(safely_constructible_from<other_type, Args...>) {
    this->clear();
    base_type::construct_other(static_cast<Args&&>(args)...);
    return this->assume_other();
  }

  template <class T, class... Args>
  requires constructible_from<T, Args...>
    and distinct_from<value_type, other_type>
    and (same_as<T, value_type> or same_as<T, other_type>)
    and (not lvalue_reference<T>)
  [[nodiscard]] value_type& emplace (::std::in_place_type_t<T>, Args&&... args)
  noexcept(safely_constructible_from<T, Args...>) {
    constexpr int index = same_as<T, other_type>;
    return this->emplace(::std::in_place_index<index>, static_cast<Args&&>(args)...);
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

} /* namespace apex */

#endif /* APEX_CORE_EITHER_HPP */
