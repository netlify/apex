#ifndef APEX_DETAIL_CORE_OPTIONAL_HPP
#define APEX_DETAIL_CORE_OPTIONAL_HPP

#include <apex/detail/prelude/trivial.hpp>
#include <apex/detail/prelude/enable.hpp>

#include <apex/core/concepts.hpp>
#include <apex/core/traits.hpp>
#include <apex/core/memory.hpp>

#include <optional>

/* forward declaration so we can use it in the monadic_base */
namespace apex { template <class> struct optional; } /* namespace apex */

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
  constexpr explicit(not sizeof...(Args)) storage_base (::std::in_place_t, Args&&... args)
  noexcept(safely_constructible_from<value_type, Args...>) :
    storage(static_cast<Args&&>(args)...),
    valid(true)
  { }

  constexpr storage_base () noexcept : valid { false } { }
  ~storage_base () noexcept(destructible<value_type>) { this->clear(); }

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
  noexcept(safely_constructible_from<value_type, Args...>) :
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
struct storage_base<T&> {
  using value_type = T&;

  constexpr storage_base (::std::in_place_t, T&&) noexcept = delete;

  constexpr storage_base (::std::in_place_t, value_type value) noexcept :
    storage(::std::addressof(value))
  { }

  constexpr storage_base () noexcept : storage { } { }
  constexpr ~storage_base () noexcept = default;

protected:
  void clear () noexcept { this->storage = nullptr; }

  T* storage;
};

template <class T>
struct access_base : storage_base<T> {
  using storage_base<T>::storage_base;
  using value_type = T;

  constexpr auto value_or (convertible_to<value_type> auto&& default_value) const&
  noexcept(safely_copy_constructible<value_type>)
  requires copy_constructible<value_type> {
    if (*this) { return **this; }
    return static_cast<value_type>(static_cast<decltype(default_value)>(default_value));
  }

  constexpr auto value_or (convertible_to<value_type> auto&& default_value) &&
  noexcept(safely_copy_constructible<value_type>)
  requires move_constructible<value_type> {
    if (*this) { return static_cast<value_type&&>(**this); }
    return static_cast<value_type>(static_cast<decltype(default_value)>(default_value));
  }

  constexpr decltype(auto) operator * () const&& noexcept { return static_cast<T const&&>(this->storage); }
  constexpr decltype(auto) operator * () const& noexcept { return static_cast<T const&>(this->storage); }
  constexpr decltype(auto) operator * () && noexcept { return static_cast<T&&>(this->storage); }
  constexpr decltype(auto) operator * () & noexcept { return static_cast<T&>(this->storage); }

  constexpr decltype(auto) value () const&& noexcept(false) {
    if (not this->has_value()) { throw std::bad_optional_access { }; }
    return static_cast<value_type const&&>(**this);
  }

  constexpr decltype(auto) value () const& noexcept(false) {
    if (not this->has_value()) { throw std::bad_optional_access { }; }
    return static_cast<value_type const&>(**this);
  }

  constexpr decltype(auto) value () && noexcept(false) {
    if (not this->has_value()) { throw std::bad_optional_access { }; }
    return static_cast<value_type&&>(**this);
  }

  constexpr decltype(auto) value () & noexcept(false) {
    if (not this->has_value()) { throw std::bad_optional_access { }; }
    return static_cast<value_type&>(**this);
  }

  constexpr bool has_value () const noexcept { return this->valid; }
};

template <class T>
struct access_base<T&> : storage_base<T&> {
  using storage_base<T&>::storage_base;

  constexpr decltype(auto) operator * () const noexcept { return static_cast<T const&>(*this->storage); }
  constexpr decltype(auto) operator * () noexcept { return static_cast<T&>(*this->storage); }

  constexpr T value_or (convertible_to<T> auto&& default_value) const noexcept {
    if (*this) { return **this; }
    return static_cast<T>(static_cast<decltype(default_value)>(default_value));
  }

  constexpr T const& value () const noexcept(false) {
    if (not this->has_value()) { throw std::bad_optional_access { }; }
    return **this;
  }

  constexpr T& value () noexcept(false) {
    if (not this->has_value()) { throw std::bad_optional_access { }; }
    return **this;
  }

  constexpr bool has_value () const noexcept { return this->storage; }
};

template <class T>
struct operations_base : access_base<T> {
  using access_base<T>::access_base;
  using value_type = T;

  template <class... Args> requires constructible_from<value_type, Args...>
  [[nodiscard]] value_type& try_emplace (Args&&... args)
  noexcept(safely_constructible_from<value_type, Args...>) {
    if (*this) { return **this; }
    this->construct(static_cast<Args&&>(args)...);
    return **this;
  }

  template <class... Args> requires safely_constructible_from<value_type, Args...>
  [[nodiscard]] value_type& emplace (Args&&... args) noexcept {
    this->clear();
    this->construct(static_cast<Args&&>(args)...);
    return **this;
  }

  template <class... Args> requires constructible_from<value_type, Args...>
  [[nodiscard]] value_type& emplace (Args&&... args) try {
    this->clear();
    this->construct(static_cast<Args&&>(args)...);
    return **this;
  } catch (...) {
    this->valid = false;
    throw;
  }

  constexpr explicit operator bool () const noexcept { return this->has_value(); }

  [[clang::reinitializes]] void reset () noexcept {
    this->clear();
    this->valid = false;
  }

protected:
  template <class... Args> requires constructible_from<T, Args...>
  constexpr void construct (Args&&... args) noexcept(safely_constructible_from<T, Args...>) {
    ::apex::construct_at(::std::addressof(**this), static_cast<Args&&>(args)...);
    this->valid = true;
  }

  template <class U>
  void assign (U&& that) {
    if (not *this and not that) { return; }
    if (*this and that) {
      return static_cast<void>(**this = *static_cast<U&&>(that));
    }
    if (*this) { return this->reset(); }
    this->construct(*static_cast<U&&>(that));
  }
};

template <class T>
struct operations_base<T&> : access_base<T&> {
  using access_base<T&>::access_base;

  constexpr explicit operator bool () const noexcept { return this->has_value(); }
  [[clang::reinitializes]] void reset () noexcept { this->clear(); }

  template <class... Args> [[nodiscard]] T& try_emplace (Args&&...) = delete;
  template <class... Args> [[nodiscard]] T& emplace (Args&&...) = delete;
};

template <class T>
struct monadic_base : operations_base<T> {
  using operations_base<T>::operations_base;

  template <class U> using optional = ::apex::optional<U>;
  template <class F, class U> using transform_type = optional<::std::invoke_result_t<F, U>>;
  using value_type = T;

  /* and_then */
  constexpr specialization_of<optional> auto and_then (invocable<value_type const&&> auto&& f) const&&
  noexcept(safely_invocable<decltype(f), value_type const&&>) {
    if (not *this) { return ::std::invoke_result_t<decltype(f), value_type const&&> { }; }
    return ::std::invoke(static_cast<decltype(f)>(f), static_cast<value_type const&&>(**this));
  }

  constexpr specialization_of<optional> auto and_then (invocable<value_type const&> auto&& f) const&
  noexcept(safely_invocable<decltype(f), value_type const&>) {
    if (not *this) { return ::std::invoke_result_t<decltype(f), value_type const&> { }; }
    return ::std::invoke(static_cast<decltype(f)>(f), **this);
  }

  constexpr specialization_of<optional> auto and_then (invocable<value_type&&> auto&& f) &&
  noexcept(safely_invocable<decltype(f), value_type&&>) {
    if (not *this) { return ::std::invoke_result_t<decltype(f), value_type&&> { }; }
    return ::std::invoke(static_cast<decltype(f)>(f), static_cast<value_type&&>(**this));
  }

  constexpr specialization_of<optional> auto and_then (invocable<value_type&> auto&& f) &
  noexcept(safely_invocable<decltype(f), value_type&>) {
    if (not *this) { return ::std::invoke_result_t<decltype(f), value_type&> { }; }
    return ::std::invoke(static_cast<decltype(f)>(f), **this);
  }

  /* transform */
  constexpr auto transform (invocable<value_type const&&> auto&& f) const&&
  noexcept(safely_invocable<decltype(f), value_type const&&>) -> transform_type<decltype(f), value_type const&&> {
    if (not *this) { return ::std::nullopt; }
    return this->apply(static_cast<decltype(f)>(f));
  }

  constexpr auto transform (invocable<value_type const&> auto&& f) const&
  noexcept(safely_invocable<decltype(f), value_type const&>) -> transform_type<decltype(f), value_type const&> {
    if (not *this) { return ::std::nullopt; }
    return this->apply(static_cast<decltype(f)>(f));
  }

  constexpr auto transform (invocable<value_type&&> auto&& f) &&
  noexcept(safely_invocable<decltype(f), value_type&&>) -> transform_type<decltype(f), value_type&&> {
    if (not *this) { return ::std::nullopt; }
    return this->apply(static_cast<decltype(f)>(f));
  }

  constexpr auto transform (invocable<value_type&> auto&& f) &
  noexcept(safely_invocable<decltype(f), value_type&>) -> transform_type<decltype(f), value_type&> {
    if (not *this) { return ::std::nullopt; }
    return this->apply(static_cast<decltype(f)>(f));
  }

  /* transform_or -- non-abbreviated form is used for readability (seriously) */
  template <invocable<value_type const&&> F, convertible_to<::std::invoke_result_t<F, value_type const&&>> U>
  constexpr transform_type<F, value_type const&&> transform_or (U&& default_value, F&& f) const&&
  noexcept(safely_invocable<F, value_type const&&>) {
    if (*this) { return ::std::invoke(static_cast<F&&>(f), static_cast<value_type const&&>(**this)); }
    return static_cast<::std::invoke_result_t<F, value_type const&&>>(static_cast<U&&>(default_value));
  }

  template <invocable<value_type const&> F, convertible_to<::std::invoke_result_t<F, value_type const&>> U>
  constexpr transform_type<F, value_type const&&> transform_or (U&& default_value, F&& f) const&
  noexcept(safely_invocable<F, value_type const&>) {
    if (*this) { return ::std::invoke(static_cast<F&&>(f), static_cast<value_type const&>(**this)); }
    return static_cast<::std::invoke_result_t<F, value_type const&&>>(static_cast<U&&>(default_value));
  }

  template <invocable<value_type&&> F, convertible_to<::std::invoke_result_t<F, value_type&&>> U>
  constexpr transform_type<F, value_type&&> transform_or (U&& default_value, F&& f) &&
  noexcept(safely_invocable<F, value_type&&>) {
    if (*this) { return ::std::invoke(static_cast<F&&>(f), static_cast<value_type&&>(**this)); }
    return static_cast<::std::invoke_result_t<F, value_type&&>>(static_cast<U&&>(default_value));
  }

  template <invocable<value_type&> F, convertible_to<::std::invoke_result_t<F, value_type&>> U>
  constexpr transform_type<F, value_type&> transform_or (U&& default_value, F&& f) &
  noexcept(safely_invocable<F, value_type&>) {
    if (*this) { return ::std::invoke(static_cast<F&&>(f), **this); }
    return static_cast<::std::invoke_result_t<F, value_type&>>(static_cast<U&&>(default_value));
  }

protected:
  constexpr auto apply (invocable<value_type const&&> auto&& f) const&&
  noexcept(safely_invocable<decltype(f), value_type const&&>) -> transform_type<decltype(f), value_type const&&> {
    return ::std::invoke(static_cast<decltype(f)>(f), static_cast<value_type const&&>(**this));
  }

  constexpr auto apply (invocable<value_type const&> auto&& f) const&
  noexcept(safely_invocable<decltype(f), value_type const&>) -> transform_type<decltype(f), value_type const&> {
    return ::std::invoke(static_cast<decltype(f)>(f), **this);
  }

  constexpr auto apply (invocable<value_type&&> auto&& f) &&
  noexcept(safely_invocable<decltype(f), value_type&&>) -> transform_type<decltype(f), value_type&&> {
    return ::std::invoke(static_cast<decltype(f)>(f), static_cast<value_type&&>(**this));
  }

  constexpr auto apply (invocable<value_type&> auto&& f) &
  noexcept(safely_invocable<decltype(f), value_type&>) -> transform_type<decltype(f), value_type&&> {
    return ::std::invoke(static_cast<decltype(f)>(f), **this);
  }
};

template <class T>
struct monadic_base<T&> : operations_base<T&> {
  using operations_base<T&>::operations_base;

  template <class U> using optional = ::apex::optional<U>;
  template <class F, class U>
  using transform_type = optional<::std::invoke_result_t<F, U>>;
  using const_reference = ::std::add_lvalue_reference_t<::std::add_const_t<T>>;
  using reference = ::std::add_lvalue_reference_t<T>;

  /* and_then */
  constexpr specialization_of<optional> auto and_then (invocable<const_reference> auto&& f) const
  noexcept(safely_invocable<const_reference>) {
    if (not *this) { return ::std::invoke_result_t<decltype(f), const_reference> { }; }
    return ::std::invoke(static_cast<decltype(f)>(f), **this);
  }

  constexpr specialization_of<optional> auto and_then (invocable<reference> auto&& f)
  noexcept(safely_invocable<reference>) {
    if (not *this) { return ::std::invoke_result_t<decltype(f), reference> { }; }
    return ::std::invoke(static_cast<decltype(f)>(f), **this);
  }

  /* transform */
  constexpr auto transform (invocable<const_reference> auto&& f) const
  noexcept(safely_invocable<decltype(f), const_reference>) -> transform_type<decltype(f), const_reference> {
    if (not *this) { return ::std::nullopt; }
    return this->apply(static_cast<decltype(f)>(f));
  }

  constexpr auto transform (invocable<reference> auto&& f)
  noexcept(safely_invocable<decltype(f), const_reference>) -> transform_type<decltype(f), reference> {
    if (not *this) { return ::std::nullopt; }
    return this->apply(static_cast<decltype(f)>(f));
  }

  /* transform_or -- non-abbreviated form helps with readability (seriously) */
  template <invocable<const_reference> F, convertible_to<::std::invoke_result_t<F, const_reference>> U>
  constexpr transform_type<F, const_reference> transform_or (U&& default_value, F&& f) const
  noexcept(safely_invocable<F, const_reference>) {
    if (*this) { return this->apply(static_cast<F&&>(f)); }
    return static_cast<::std::invoke_result_t<F, const_reference&>>(static_cast<U&&>(default_value));
  }

  template <invocable<reference> F, convertible_to<::std::invoke_result_t<F, reference>> U>
  constexpr transform_type<F, reference> transform_or (U&& default_value, F&& f)
  noexcept(safely_invocable<F, reference>) {
    if (*this) { return this->apply(static_cast<F>(f)); }
    return static_cast<::std::invoke_result_t<F, reference>>(static_cast<U&&>(default_value));
  }

protected:
  constexpr auto apply (invocable<const_reference> auto&& f) const
  noexcept(safely_invocable<decltype(f), const_reference>) -> transform_type<decltype(f), const_reference> {
    return ::std::invoke(static_cast<decltype(f)>(f), **this);
  }

  constexpr auto apply (invocable<reference> auto&& f)
  noexcept(safely_invocable<decltype(f), reference>) -> transform_type<decltype(f), reference> {
    return ::std::invoke(static_cast<decltype(f)>(f), **this);
  }
};

template <class T>
struct copy_constructible_base : monadic_base<T> {
  using monadic_base<T>::monadic_base;

  constexpr copy_constructible_base (copy_constructible_base const& that)
  noexcept(safely_copy_constructible<T>) { this->construct(*that); }
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
  noexcept(safely_move_constructible<T>) { this->construct(static_cast<T&&>(*that)); }
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

  copy_assignable_base& operator = (copy_assignable_base const& that) noexcept(safely_copy_assignable<T>) {
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
  move_assignable_base& operator = (move_assignable_base&& that) noexcept(safely_move_assignable<T>) {
    this->assign(static_cast<move_assignable_base&&>(that));
    return *this;
  }
};

template <trivially_copy_constructible T>
struct copy_constructible_base<T> : monadic_base<T> {
  using monadic_base<T>::monadic_base;
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
struct copy_constructible_base<T&> : monadic_base<T&> {
  using monadic_base<T&>::monadic_base;
};

template <class T>
struct move_constructible_base<T&> : copy_constructible_base<T&> {
  using copy_constructible_base<T&>::copy_constructible_base;
};

template <class T>
struct copy_assignable_base<T&> : move_constructible_base<T&> {
  using move_constructible_base<T&>::move_constructible_base;
};

template <class T>
struct move_assignable_base<T&> : copy_assignable_base<T&> {
  using copy_assignable_base<T&>::copy_assignable_base;
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
