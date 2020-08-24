#ifndef APEX_DETAIL_CORE_OPTIONAL_HPP
#define APEX_DETAIL_CORE_OPTIONAL_HPP

#include <apex/detail/prelude/trivial.hpp>
#include <apex/detail/prelude/enable.hpp>

#include <apex/core/concepts.hpp>
#include <apex/core/traits.hpp>
#include <apex/core/memory.hpp>

#include <optional>

namespace apex::detail::optional {

template <class T, class U>
static constexpr bool different_value = requires {
  requires ::std::is_constructible_v<T, U>;
  requires ::std::is_assignable_v<T, U>;
  requires not ::std::is_scalar_v<T> or distinct_from<::std::decay_t<U>, T>;
};

template <class T>
struct storage_base {
  using value_type = T;

  template <class... Args> requires constructible_from<value_type, Args...>
  constexpr storage_base (::std::in_place_t, Args&&... args)
  noexcept(::std::is_nothrow_constructible_v<value_type, Args...>) :
    storage(static_cast<Args&&>(args)...),
    valid(true)
  { }

  constexpr storage_base () noexcept : valid { false } { }
  ~storage_base () noexcept(::std::is_nothrow_destructible_v<value_type>) {
    this->clear();
  }

protected:
  void clear () noexcept {
    if (this->valid) { ::apex::destroy_at(::std::addressof(this->storage)); }
  }

  union { value_type storage; };
  bool valid;
};

template <trivially_destructible T>
struct storage_base<T> {
  using value_type = T;

  template <class... Args> requires constructible_from<value_type, Args...>
  constexpr storage_base (::std::in_place_t, Args&&... args)
  noexcept(::std::is_nothrow_constructible_v<value_type, Args...>) :
    storage(static_cast<Args&&>(args)...),
    valid(true)
  { }

  constexpr storage_base () noexcept :
    valid { false }
  { }

  ~storage_base () noexcept = default;

protected:
  void clear () noexcept { }

  union { value_type storage; };
  bool valid { };
};

template <class T>
struct operations_base : storage_base<T> {
  using storage_base<T>::storage_base;

  constexpr explicit operator bool () const noexcept { return this->has_value(); }
  constexpr bool has_value () const noexcept { return this->valid; }

  [[clang::reinitializes]] void reset () noexcept {
    this->clear();
    this->valid = false;
  }

protected:
  template <class... Args> requires constructible_from<T, Args...>
  constexpr void construct (Args&&... args)
  noexcept(::std::is_nothrow_constructible_v<T, Args...>) {
    ::apex::construct_at(::std::addressof(this->get()), static_cast<Args&&>(args)...);
    this->valid = true;
  }

  template <class U>
  void assign (U&& that) {
    if (not *this and not that) { return; }
    if (*this and that) {
      return static_cast<void>(this->get() = static_cast<U&&>(that).get());
    }
    if (*this) { return this->reset(); }
    this->construct(static_cast<U&&>(that).get());
  }

  // TODO: Investigate whether this requires the use of std::launder
  void clear () noexcept {
    if (*this) { ::apex::destroy_at(::std::addressof(this->get())); }
  }

  T const&& get () const&& noexcept { return static_cast<xvalue_of<T const>>(this->storage); }
  T const& get () const& noexcept { return this->storage; }
  T&& get () && noexcept { return static_cast<xvalue_of<T>>(this->storage); }
  T& get () & noexcept { return this->storage; }
};

template <class T>
struct copy_constructible_base : operations_base<T> {
  using operations_base<T>::operations_base;

  constexpr copy_constructible_base (copy_constructible_base const& that)
  noexcept(::std::is_nothrow_copy_constructible_v<T>) {
    if (that) { this->construct(that.get()); }
  }
  constexpr copy_constructible_base (copy_constructible_base&&) noexcept = default;
  constexpr copy_constructible_base () noexcept = default;

  constexpr copy_constructible_base& operator = (copy_constructible_base const&) noexcept = default;
  constexpr copy_constructible_base& operator = (copy_constructible_base&&) noexcept = default;
};

template <class T>
struct move_constructible_base : copy_constructible_base<T> {
  using copy_constructible_base<T>::copy_constructible_base;

  constexpr move_constructible_base (move_constructible_base const&) noexcept = default;
  constexpr move_constructible_base (move_constructible_base&& that)
  noexcept(::std::is_nothrow_move_constructible_v<T>) {
    if (that) { this->construct(static_cast<T&&>(that.get())); }
  }
  constexpr move_constructible_base () noexcept = default;

  constexpr move_constructible_base& operator = (move_constructible_base const&) noexcept = default;
  constexpr move_constructible_base& operator = (move_constructible_base&&) noexcept = default;
};

template <class T>
struct copy_assignable_base : move_constructible_base<T> {
  using move_constructible_base<T>::move_constructible_base;

  constexpr copy_assignable_base (copy_assignable_base const&) = default;
  constexpr copy_assignable_base (copy_assignable_base&&) = default;
  constexpr copy_assignable_base () = default;

  copy_assignable_base& operator = (copy_assignable_base const& that) noexcept(nothrow_copy_assignable<T>) {
    this->assign(that);
    return *this;
  }
  copy_assignable_base& operator = (copy_assignable_base&&) = default;
};

template <class T>
struct move_assignable_base : copy_assignable_base<T> {
  using copy_assignable_base<T>::copy_assignable_base;

  constexpr move_assignable_base (move_assignable_base const&) = default;
  constexpr move_assignable_base (move_assignable_base&&) = default;
  constexpr move_assignable_base () = default;

  move_assignable_base& operator = (move_assignable_base const&) = default;
  move_assignable_base& operator = (move_assignable_base&& that) noexcept(nothrow_move_assignable<T>) {
    this->assign(static_cast<move_assignable_base&&>(that));
    return *this;
  }
};

template <trivially_copy_constructible T>
struct copy_constructible_base<T> : operations_base<T> {
  using operations_base<T>::operations_base;
};

template <trivially_move_constructible T>
struct move_constructible_base<T> : copy_constructible_base<T> {
  using copy_constructible_base<T>::copy_constructible_base;
};

template <trivially_copy_assignable T>
struct copy_assignable_base<T> : move_constructible_base<T> {
  using move_constructible_base<T>::move_constructible_base;
};

template <trivially_move_assignable T>
struct move_assignable_base<T> : copy_assignable_base<T> {
  using copy_assignable_base<T>::copy_assignable_base;
};

template <class T>
using constructor = prelude::enable::constructor<
  ::std::is_copy_constructible_v<T>,
  ::std::is_move_constructible_v<T>
>;

template <class T>
using assignment = prelude::enable::assignment<
  ::std::conjunction_v<::std::is_copy_constructible<T>, ::std::is_copy_assignable<T>>,
  ::std::conjunction_v<::std::is_move_constructible<T>, ::std::is_move_assignable<T>>
>;

template <class T>
struct base : move_assignable_base<T>, constructor<T>, assignment<T> {
  using move_assignable_base<T>::move_assignable_base;
};

} /* namespace apex::detail::optional */

#endif /* APEX_DETAIL_CORE_OPTIONAL_HPP */
