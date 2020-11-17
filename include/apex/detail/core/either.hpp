#ifndef APEX_DETAIL_CORE_EITHER_HPP
#define APEX_DETAIL_CORE_EITHER_HPP

#include <apex/detail/prelude/reference.hpp>
#include <apex/detail/prelude/trivial.hpp>
#include <apex/detail/prelude/enable.hpp>
#include <apex/detail/prelude/types.hpp>

#include <apex/core/concepts.hpp>
#include <apex/core/traits.hpp>
#include <apex/core/memory.hpp>
#include <apex/core/bit.hpp>

#include <utility>

namespace apex::detail::either {

struct value_t { };
struct other_t { };

inline constexpr value_t value { };
inline constexpr other_t other { };

using prelude::lvalue_reference;

template <class T> concept trivial_storage = lvalue_reference<T>
  or trivially_destructible<T>;

template <class T> using storage_t = ::std::conditional_t<
  lvalue_reference<T>,
  ::std::remove_reference_t<T>*,
  T
>;

/* TODO: Move to a detail/core/monads.hpp header? */
template <class T, class U>
concept safely_assign_either = requires (T x, U y) {
  { x.assign_value_with(static_cast<U&&>(y).assume_value()) } noexcept;
  { x.assign_value(static_cast<U&&>(y)) } noexcept;

  { x.assign_other_with(static_cast<U&&>(y).assume_other()) } noexcept;
  { x.assign_other(static_cast<U&&>(y)) } noexcept;
};

/* storage_base */
template <class A, class B>
struct storage_base {
  using value_type = A;
  using other_type = B;

  constexpr storage_base () = delete;
  constexpr ~storage_base () noexcept(noexcept(this->clear())) { this->clear(); }

protected:
  using storage_value_type = storage_t<value_type>;
  using storage_other_type = storage_t<other_type>;

  template <class... Args> requires constructible_from<storage_value_type, Args...>
  constexpr explicit(not sizeof...(Args)) storage_base (value_t, Args&&... args)
  noexcept(safely_constructible_from<storage_value_type, Args...>) :
    primary(static_cast<Args&&>(args)...),
    which(0)
  { }

  template <class... Args> requires constructible_from<storage_other_type, Args...>
  constexpr explicit(not sizeof...(Args)) storage_base (other_t, Args&&... args)
  noexcept(safely_constructible_from<storage_other_type, Args...>) :
    another(static_cast<Args&&>(args)...),
    which(1)
  { }

  constexpr void clear_value () noexcept(destructible<value_type>) {
    ::apex::destroy_at(::std::addressof(this->primary));
  }

  constexpr void clear_other () noexcept(destructible<other_type>) {
    ::apex::destroy_at(::std::addressof(this->another));
  }

  constexpr void clear () noexcept(destructible<value_type> and destructible<other_type>) {
    if (this->which == 0) { return this->clear_value(); }
    if (this->which == 1) { return this->clear_other(); }
    apex::unreachable();
  }

  union {
    storage_value_type primary;
    storage_other_type another;
  };
  u8 which;
};

template <trivial_storage A, trivial_storage B>
struct storage_base<A, B> {
  using value_type = A;
  using other_type = B;

  constexpr storage_base () = delete;
  constexpr ~storage_base () noexcept = default;

protected:
  using storage_value_type = storage_t<value_type>;
  using storage_other_type = storage_t<other_type>;

  template <class... Args> requires constructible_from<storage_value_type, Args...>
  constexpr explicit(not sizeof...(Args)) storage_base (value_t, Args&&... args)
  noexcept(safely_constructible_from<storage_value_type, Args...>) :
    primary(static_cast<Args&&>(args)...),
    which(0)
  { }

  template <class... Args> requires constructible_from<storage_other_type, Args...>
  constexpr explicit(not sizeof...(Args)) storage_base (other_t, Args&&... args)
  noexcept(safely_constructible_from<storage_other_type, Args...>) :
    another(static_cast<Args&&>(args)...),
    which(1)
  { }

  constexpr void clear () noexcept { }

  union {
    storage_value_type primary;
    storage_other_type another;
  };
  u8 which;
};

template <class A, class B>
struct access_base : storage_base<A, B> {
  using storage_base<A, B>::storage_base;

  constexpr decltype(auto) assume_value () const&& noexcept {
    if constexpr (lvalue_reference<A>) { return ::std::as_const(*this->primary); }
    else { return static_cast<A const&&>(this->primary); }
  }

  constexpr decltype(auto) assume_value () const& noexcept {
    if constexpr (lvalue_reference<A>) { return ::std::as_const(*this->primary); }
    else { return static_cast<A const&>(this->primary); }
  }

  constexpr decltype(auto) assume_value () && noexcept {
    if constexpr (lvalue_reference<A>) { return *this->primary; }
    else { return static_cast<A&&>(this->primary); }
  }

  constexpr decltype(auto) assume_value () & noexcept {
    if constexpr (lvalue_reference<A>) { return *this->primary; }
    else { return static_cast<A&>(this->primary); }
  }

  constexpr decltype(auto) assume_other () const&& noexcept {
    if constexpr (lvalue_reference<B>) { return ::std::as_const(*this->another); }
    else { return static_cast<B const&&>(this->another); }
  }

  constexpr decltype(auto) assume_other () const& noexcept {
    if constexpr(lvalue_reference<B>) { return ::std::as_const(*this->another); }
    else { return static_cast<B const&>(this->another); }
  }

  constexpr decltype(auto) assume_other () && noexcept {
    if constexpr(lvalue_reference<B>) { return *this->another; }
    else { return static_cast<B&&>(this->another); }
  }

  constexpr decltype(auto) assume_other () & noexcept {
    if constexpr(lvalue_reference<B>) { return *this->another; }
    else { return static_cast<B&>(this->another); }
  }
};

template <class A, class B>
struct operations_base : access_base<A, B> {
  using access_base<A, B>::access_base;

  [[nodiscard]] constexpr bool has_value () const noexcept { return this->index() == 0; }
  [[nodiscard]] constexpr bool has_other () const noexcept { return this->index() == 1; }

  [[nodiscard]] constexpr int index () const noexcept { return this->which; }

protected:
  template <class... Args> requires constructible_from<A, Args...>
  constexpr void construct_value (Args&&... args) noexcept(safely_constructible_from<A, Args...>) {
    ::apex::construct_at(::std::addressof(this->assume_value()), static_cast<Args&&>(args)...);
    this->which = 0;
  }

  template <class... Args> requires constructible_from<B, Args...>
  constexpr void construct_other (Args&&... args) noexcept(safely_constructible_from<B, Args...>) {
    ::apex::construct_at(::std::addressof(this->assume_other()), static_cast<Args&&>(args)...);
    this->which = 1;
  }

  // TODO: set correct noexcept expression
  template <class T>
  void assign (T&& that) noexcept(safely_assign_either<operations_base, T>) {
    if (this->has_value() and that.has_value()) { return this->assign_value(static_cast<T&&>(that)); }
    if (this->has_other() and that.has_other()) { return this->assign_other(static_cast<T&&>(that)); }
    if (this->has_value() and that.has_other()) {
      return this->assign_other_with(static_cast<T&&>(that).assume_other());
    }
    this->assign_value_with(static_cast<T&&>(that).assume_value());
  }

  template <class T>
  void assign_value (T&& that)
  noexcept(noexcept(this->assume_value() = static_cast<T&&>(that).assume_value())) {
    this->assume_value() = static_cast<T&&>(that).assume_value();
  }

  template <class T>
  void assign_other (T&& that)
  noexcept(noexcept(this->assume_other() = static_cast<T&&>(that).assume_other())) {
    this->assume_other() = static_cast<T&&>(that).assume_other();
  }

  template <class T> requires safely_constructible_from<A, T>
  void assign_value_with (T&& value) noexcept {
    this->clear();
    this->construct_value(static_cast<T&&>(value));
  }

  template <class T> requires safely_constructible_from<B, T>
  void assign_other_with (T&& value) noexcept {
    this->clear();
    this->construct_other(static_cast<T&&>(value));
  }

  template <class T> requires (not safely_constructible_from<A, T>)
  void assign_value_with (T&& value) noexcept(false) {
    auto backup = static_cast<operations_base&&>(*this).assume_other();
    this->clear();
    try { this->construct_value(static_cast<T&&>(value)); }
    catch (...) {
      this->assume_other() = static_cast<B&&>(backup);
      throw;
    }
  }

  template <class T> requires (not safely_constructible_from<B, T>)
  void assign_other_with (T&& value) noexcept(false) {
    auto backup = static_cast<operations_base&&>(*this).assume_other();
    this->clear();
    try { this->construct_other(static_cast<T&&>(value)); }
    catch (...) {
      this->assume_value() = static_cast<A&&>(backup);
      throw;
    }
  }
};

template <class A, class B>
struct copy_constructible_base : operations_base<A, B> {
  using operations_base<A, B>::operations_base;

  constexpr copy_constructible_base (copy_constructible_base const& that)
  noexcept(safely_copy_constructible<A> and safely_copy_constructible<B>) {
    if (that.has_value()) { return this->construct_value(that.assume_value()); }
    this->construct_other(that.assume_other());
  }

  constexpr copy_constructible_base (copy_constructible_base&&) = default;
  constexpr copy_constructible_base () = default;

  constexpr copy_constructible_base& operator = (copy_constructible_base const&) = default;
  constexpr copy_constructible_base& operator = (copy_constructible_base&&) = default;
};

template <class A, class B>
struct move_constructible_base : copy_constructible_base<A, B> {
  using copy_constructible_base<A, B>::copy_constructible_base;

  constexpr move_constructible_base (move_constructible_base&& that)
  noexcept(safely_move_constructible<A> and safely_move_constructible<B>) {
    if (that.has_value()) {
      return this->construct_value(static_cast<move_constructible_base&&>(that).assume_value());
    }
    this->construct_other(static_cast<move_constructible_base&&>(that).assume_other());
  }

  constexpr move_constructible_base (move_constructible_base const&) = default;
  constexpr move_constructible_base () = default;

  constexpr move_constructible_base& operator = (move_constructible_base const&) = default;
  constexpr move_constructible_base& operator = (move_constructible_base&&) = default;
};

template <class A, class B>
struct copy_assignable_base : move_constructible_base<A, B> {
  using move_constructible_base<A, B>::move_constructible_base;

  constexpr copy_assignable_base& operator = (copy_assignable_base const& that)
  noexcept(noexcept(this->assign(that))) {
    this->assign(that);
    return *this;
  }

  constexpr copy_assignable_base (copy_assignable_base const&) = default;
  constexpr copy_assignable_base (copy_assignable_base&&) = default;
  constexpr copy_assignable_base () = default;

  constexpr copy_assignable_base& operator = (copy_assignable_base&&) = default;
};

template <class A, class B>
struct move_assignable_base : copy_assignable_base<A, B> {
  using copy_assignable_base<A, B>::copy_assignable_base;

  constexpr move_assignable_base& operator = (move_assignable_base&& that)
  noexcept(noexcept(this->assign(static_cast<move_assignable_base&&>(that)))) {
    this->assign(static_cast<move_assignable_base&&>(that));
    return *this;
  }

  constexpr move_assignable_base (move_assignable_base const&) = default;
  constexpr move_assignable_base (move_assignable_base&&) = default;
  constexpr move_assignable_base () = default;

  constexpr move_assignable_base& operator = (move_assignable_base const&) = default;
};

template <trivially_copy_constructible A, trivially_copy_constructible B>
struct copy_constructible_base<A, B> : operations_base<A, B> {
  using operations_base<A, B>::operations_base;
};

template <trivially_move_constructible A, trivially_move_constructible B>
struct move_constructible_base<A, B> : copy_constructible_base<A, B> {
  using copy_constructible_base<A, B>::copy_constructible_base;
};

template <trivially_copy_assignable A, trivially_copy_assignable B>
struct copy_assignable_base<A, B> : move_constructible_base<A, B> {
  using move_constructible_base<A, B>::move_constructible_base;
};

template <trivially_move_assignable A, trivially_move_assignable B>
struct move_assignable_base<A, B> : copy_assignable_base<A, B> {
  using copy_assignable_base<A, B>::copy_assignable_base;
};

template <class A, class B>
struct base :
  move_assignable_base<A, B>,
  prelude::enable::constructor<A, B>,
  prelude::enable::assignment<A, B>
{ using move_assignable_base<A, B>::move_assignable_base; };

} /* namespace apex::detail::either */

#endif /* APEX_DETAIL_CORE_EITHER_HPP */
