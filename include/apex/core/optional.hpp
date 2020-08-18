#ifndef APEX_CORE_OPTIONAL_HPP
#define APEX_CORE_OPTIONAL_HPP

#include <apex/detail/prelude/enable.hpp>
#include <apex/detail/core/optional.hpp>

// We implement our own optional because std::optional
// 1) cannot be specialized
// 2) Does not support references due to a lot of drama.
// 3) Is not as conditionally noexcept as it can be
// 4) Does not (currently) use concepts, and is thus under-constrained

// TODO: The following operations must be rewritten to follow the correct
//       style when writing transforms, and_then, etc.
// 1) and_then -> MUST return an optional
// 2) or_else -> convertible_to<::std::invoke_result_t<F>, optional<T>> OR void
// 3) transform -> functions returns T, is placed into an optional
// 4) transform_or -> function returns T, is placed into an optional.
//                    default_value placed into an optional
// 5) transform_or_else -> function returns T, is placed into an optional
//                         default_value placed into an optional
// Additional operations we could take from Rust:
//  * template <predicate<value_type> F> optional filter (F&& f) noexcept(...);
//  * optional<typename value_type::value_type> flatten () noexcept(...);
//  * as_ref -> optional<value_type&>;
// Eventually, we would want to add the ok_or, ok_or_else, and friends that
// are also found in Rust.
// Additionally, we'll want to eventually add an iterator wrapper,
// either view::maybe<T> *or* just have a .inspect member function
namespace apex {

/** @brief Manages an *optional* value. Similar to std::optional
 * @monad
 * Unlike C++'s standard library, a few additional extensions are added. These
 * include monadic operations typically found in other programming languages
 * such as Rust and Haskell, as well as the ability to have an `optional<T&>`
 * with a slightly reduced interface. e.g., one can #transform the value stored
 * within an optional, returning a new optional with the resulting value stored
 * in it, or an empty optional if the original was also empty.
 */
template <class> struct optional;

template <class T>
struct optional final : private detail::optional::base<T> {
  using value_type = T;
  using base_type = detail::optional::base<T>;

  template <class F, class U>
  using transform_type = optional<::std::invoke_result_t<F, U>>;

  template <class... Args> requires constructible_from<value_type, Args...>
  constexpr explicit(not sizeof...(Args)) optional (::std::in_place_t, Args&&... args)
  noexcept(::std::is_nothrow_constructible_v<value_type, Args...>) :
    base_type(::std::in_place, static_cast<Args&&>(args)...)
  { }

  template <class U=value_type>
  requires different_from<optional, U> and ::std::is_constructible_v<value_type, U>
  constexpr explicit(not ::std::is_convertible_v<U, value_type>) optional (U&& value)
    noexcept(::std::is_nothrow_constructible_v<value_type, U>) :
    optional { std::in_place, static_cast<U&&>(value) }
  { }

  constexpr optional (::std::nullopt_t) noexcept :
    optional { }
  { }

  constexpr optional (optional const&) = default;
  constexpr optional (optional&&) = default;
  constexpr optional () noexcept = default;

  template <class U=value_type>
  requires different_from<optional, U>
  and ::std::is_constructible_v<value_type, U>
  and ::std::is_assignable_v<value_type, U>
  optional& operator = (U&& value) noexcept {
    if (*this) { **this = static_cast<U&&>(value); }
    else { this->construct(static_cast<U&&>(value)); }
    return *this;
  }

  optional& operator = (::std::nullopt_t) noexcept {
    this->reset();
    return *this;
  }

  optional& operator = (optional const&) = default;
  optional& operator = (optional&&) = default;

  friend void swap (optional& lhs, optional& rhs) noexcept(noexcept(apex::ranges::swap)) {
    if (not lhs and not rhs) { return; }
    if (lhs and rhs) { return apex::ranges::swap(*lhs, *rhs); }
    auto& uninitialized = lhs ? rhs : lhs;
    auto& initialized = lhs ? lhs : rhs;
    uninitialized = static_cast<xvalue_of<value_type>>(lhs);
    initialized = ::std::nullopt;
  }

  constexpr explicit operator bool () const noexcept { return this->has_value(); }

  constexpr value_type const&& operator * () const&& noexcept {
    return static_cast<xvalue_of<value_type const>>(this->get());
  }

  constexpr value_type&& operator * () && noexcept {
    return static_cast<xvalue_of<value_type>>(this->get());
  }

  constexpr value_type const& operator * () const& noexcept { return this->get(); }
  constexpr value_type& operator * () & noexcept { return this->get(); }

  constexpr value_type const* operator -> () const noexcept { return ::std::addressof(**this); }
  constexpr value_type* operator -> () noexcept { return ::std::addressof(**this); }

  [[clang::reinitializes]] void reset () noexcept { base_type::reset(); }

  template <class... Args> requires constructible_from<value_type, Args...>
  [[nodiscard]] value_type& try_emplace (Args&&... args)
  noexcept(::std::is_nothrow_constructible_v<value_type, Args...>) {
    if (*this) { return **this; }
    this->construct(static_cast<Args&&>(args)...);
    return **this;
  }

  template <class... Args> requires constructible_from<value_type, Args...>
  value_type& emplace (Args&&... args)
  noexcept(::std::is_nothrow_constructible_v<value_type, Args...>) try {
    this->clear();
    this->construct(static_cast<Args&&>(args)...);
    return **this;
  } catch (...) {
    this->valid = false;
    throw;
  }

  constexpr value_type const&& value () const&& noexcept(false) {
    if (not *this) { throw std::bad_optional_access { }; }
    return std::move(**this);
  }

  constexpr value_type const& value () const& noexcept(false) {
    if (not *this) { throw std::bad_optional_access { }; }
    return **this;
  }

  constexpr value_type&& value () && noexcept(false) {
    if (not *this) { throw std::bad_optional_access { }; }
    return std::move(**this);
  }

  constexpr value_type& value () & noexcept(false) {
    if (not *this) { throw std::bad_optional_access { }; }
    return **this;
  }

  template <convertible_to<value_type> U> requires copy_constructible<value_type>
  constexpr value_type value_or (U&& default_value) const&
    noexcept(::std::is_nothrow_copy_constructible_v<value_type>)
  {
    if (*this) { return **this; }
    return static_cast<value_type>(static_cast<U&&>(default_value));
  }

  template <convertible_to<value_type> U> requires move_constructible<value_type>
  constexpr value_type value_or (U&& default_value) &&
    noexcept(::std::is_nothrow_move_constructible_v<value_type>)
  {
    if (*this) { return static_cast<xvalue_of<value_type>>(**this); }
    return static_cast<value_type>(static_cast<U&&>(default_value));
  }

  template <invocable<value_type const&&> F>
  requires is_specialization_of_v<optional, ::std::invoke_result_t<F, value_type const&&>>
  constexpr ::std::invoke_result_t<F, value_type const&&> and_then (F&& f) const&&
  noexcept(::std::is_nothrow_invocable_v<F, value_type const&&>) {
    if (not *this) { return std::nullopt; }
    return ::std::invoke(static_cast<F&&>(f), static_cast<xvalue_of<value_type>>(**this));
  }

  template <invocable<value_type const&> F>
  requires is_specialization_of_v<optional, ::std::invoke_result_t<F, value_type const&>>
  constexpr ::std::invoke_result_t<F, value_type const&> and_then (F&& f) const&
  noexcept(::std::is_nothrow_invocable_v<F, value_type const&>) {
    if (not *this) { return std::nullopt; }
    return ::std::invoke(static_cast<F&&>(f), **this);
  }

  template <invocable<value_type&&> F>
  requires is_specialization_of_v<optional, ::std::invoke_result_t<F, value_type&&>>
  constexpr ::std::invoke_result_t<F, value_type&&> and_then (F&& f) &&
  noexcept(::std::is_nothrow_invocable_v<F, value_type&&>) {
    if (not *this) { return std::nullopt; }
    return ::std::invoke(static_cast<F&&>(f), static_cast<xvalue_of<value_type>>(**this));
  }

  template <invocable<value_type&> F>
  requires is_specialization_of_v<optional, ::std::invoke_result_t<F, value_type&&>>
  constexpr ::std::invoke_result_t<F, value_type&> and_then (F&& f) &
  noexcept(::std::is_nothrow_invocable_v<F, value_type&&>) {
    if (not *this) { return std::nullopt; }
    return ::std::invoke(static_cast<F&&>(f), **this);
  }

  template <invocable<value_type const&&> F>
  constexpr transform_type<F, value_type const&&> transform (F&& f) const&&
  noexcept(::std::is_nothrow_invocable_v<F, value_type const&&>) {
    if (not *this) { return ::std::nullopt; }
    return ::std::invoke(static_cast<F&&>(f), static_cast<xvalue_of<value_type const>>(**this));
  }

  template <invocable<value_type const&> F>
  constexpr transform_type<F, value_type const&> transform (F&& f) const&
  noexcept(::std::is_nothrow_invocable_v<F, value_type const&>) {
    if (not *this) { return ::std::nullopt; }
    return ::std::invoke(static_cast<F&&>(f), **this);
  }

  template <invocable<value_type&&> F>
  constexpr transform_type<F, value_type&&> transform (F&& f) &&
  noexcept(::std::is_nothrow_invocable_v<F, value_type&&>) {
    if (not *this) { return ::std::nullopt; }
    return ::std::invoke(static_cast<F&&>(f), static_cast<xvalue_of<value_type>>(**this));
  }

  template <invocable<value_type&> F>
  constexpr transform_type<F, value_type&> transform (F&& f) &
  noexcept(::std::is_nothrow_invocable_v<F, value_type&>) {
    if (not *this) { return ::std::nullopt; }
    return ::std::invoke(static_cast<F&&>(f), **this);
  }

  template <invocable F>
  constexpr optional or_else (F&& f) const&& noexcept(::std::is_nothrow_invocable_v<F>) {
    if (*this) { return std::move(*this); }
    return this->otherwise(static_cast<F&&>(f));
  }

  template <invocable F>
  constexpr optional or_else (F&& f) const& noexcept(::std::is_nothrow_invocable_v<F>) {
    if (*this) { return *this; }
    return this->otherwise(static_cast<F&&>(f));
  }

  template <invocable F>
  constexpr optional or_else (F&& f) && noexcept(::std::is_nothrow_invocable_v<F>) {
    if (*this) { return std::move(*this); }
    return this->otherwise(static_cast<F&&>(f));
  }

  template <invocable F>
  constexpr optional or_else (F&& f) & noexcept(::std::is_nothrow_invocable_v<F>) {
    if (*this) { return *this; }
    return this->otherwise(static_cast<F&&>(f));
  }
private:
  template <invocable F>
  constexpr optional otherwise (F&& f) const noexcept {
    if constexpr (not is_void_v<::std::invoke_result_t<F>>) {
      return optional { ::std::invoke(static_cast<F&&>(f)) };
    } else {
      ::std::invoke(static_cast<F&&>(f));
      return ::std::nullopt;
    }
  }
};

template <class T>
struct optional<T&> final {
  using storage_type = ::std::add_pointer_t<::std::remove_reference_t<T>>;
  using value_type = ::std::add_lvalue_reference_t<T>;

  using const_reference = ::std::add_lvalue_reference_t<::std::add_const_t<T>>;
  using reference = ::std::add_lvalue_reference_t<T>;

  template <class F, class U>
  using transform_type = optional<::std::invoke_result_t<F, U>>;

  template <class U>
  static constexpr auto different_reference = requires {
    requires convertible_to<U, value_type>;
    requires ::std::is_lvalue_reference_v<U>;
  };

  template <class F, class DF, class U>
  static constexpr auto transform_other = ::std::is_void_v<::std::invoke_result_t<DF>>
    or  convertible_to<::std::invoke_result_t<DF>, typename transform_type<F, U>::value_type>;

  template <class U> requires different_reference<U>
  constexpr explicit optional (remove_reference_t<U>& ref) noexcept :
    storage(::std::addressof(ref))
  { }

  template <class U> requires convertible_to<add_lvalue_reference_t<U>, value_type>
  constexpr explicit optional (::std::reference_wrapper<U> ref) noexcept :
    optional { ref.get() }
  { }

  constexpr explicit optional (value_type ref) noexcept :
    storage(::std::addressof(ref))
  { }

  constexpr optional (::std::in_place_t, ::std::reference_wrapper<T> ref) noexcept :
    optional(ref.get())
  { }

  constexpr optional (::std::in_place_t, value_type ref) noexcept :
    optional(ref)
  { }

  constexpr optional (optional const&) noexcept = default;
  constexpr optional (T&&) = delete;
  constexpr optional () noexcept = default;

  constexpr optional& operator = (optional const&) noexcept = default;
  constexpr optional& operator = (std::nullopt_t) noexcept {
    this->reset();
    return *this;
  }

  // TODO: make this a friend function~
  constexpr void swap (optional& that) noexcept {
    apex::ranges::swap(this->storage, that.storage);
  }

  constexpr explicit operator bool () const noexcept { return this->storage; }

  constexpr decltype(auto) operator * () const noexcept { return *this->storage; }
  constexpr auto operator -> () const noexcept { return ::std::addressof(**this); }

  constexpr bool has_value () const noexcept { return static_cast<bool>(*this); }
  [[clang::reinitializes]] constexpr void reset () noexcept { this->storage = nullptr; }

  constexpr value_type value () const noexcept(false) {
    if (not *this) { throw std::bad_optional_access { }; }
    return **this;
  }

  template <convertible_to<value_type> U>
  constexpr value_type value_or (U&& default_value) const noexcept {
    if (*this) { return **this; }
    return static_cast<value_type>(static_cast<U&&>(default_value));
  }

  template <invocable<const_reference> F>
  constexpr transform_type<F, const_reference> transform (F&& f) const
  noexcept(is_nothrow_invocable_v<F, const_reference>) {
    if (not *this) { return std::nullopt; }
    return ::std::invoke(static_cast<F&&>(f), **this);
  }

  template <invocable<reference> F>
  constexpr transform_type<F, reference> transform (F&& f)
  noexcept(is_nothrow_invocable_v<F, reference>) {
    if (not *this) { return std::nullopt; }
    return ::std::invoke(static_cast<F&&>(f), **this);
  }

  template <invocable<const_reference> F>
  requires is_specialization_of_v<optional, ::std::invoke_result_t<F, const_reference>>
  constexpr ::std::invoke_result_t<F, const_reference> and_then (F&& f) const
  noexcept(is_nothrow_invocable_v<F, const_reference>) {
    if (not *this) { return std::nullopt; }
    return ::std::invoke(static_cast<F&&>(f), **this);
  }

  template <invocable<reference> F>
  requires is_specialization_of_v<optional, ::std::invoke_result_t<F, reference>>
  constexpr ::std::invoke_result_t<F, reference> and_then (F&& f)
  noexcept(is_nothrow_invocable_v<F, reference>) {
    if (not *this) { return std::nullopt; }
    return ::std::invoke(static_cast<F&&>(f), **this);
  }

  template <invocable F>
  constexpr optional or_else (F&& f) const
  noexcept(noexcept(this->otherwise(static_cast<F&&>(f)))) {
    if (*this) { return *this; }
    return this->otherwise(static_cast<F&&>(f));
  }

  template <invocable F>
  constexpr optional or_else (F&& f) noexcept(noexcept(this->otherwise(static_cast<F&&>(f)))) {
    if (*this) { return *this; }
    return this->otherwise(static_cast<F&&>(f));
  }

  template <invocable<const_reference> F, convertible_to<::std::invoke_result_t<F, const_reference>> U>
  constexpr transform_type<F, const_reference> transform_or (F&& f, U&& default_value) 
  noexcept(is_nothrow_invocable_v<F, const_reference>) {
    if (*this) { return ::std::invoke(static_cast<F&&>(f), **this); }
    return static_cast<::std::invoke_result_t<F, const_reference>>(static_cast<U&&>(default_value));
  }

  template <invocable<reference> F, convertible_to<::std::invoke_result_t<F, reference>> U>
  constexpr transform_type<F, reference> transform_or (F&& f, U&& default_value)
  noexcept(is_nothrow_invocable_v<F, reference>) {
    if (*this) { return ::std::invoke(static_cast<F&&>(f), **this); }
    return static_cast<::std::invoke_result_t<F, reference>>(static_cast<U&&>(default_value));
  }

  template <invocable<const_reference> F, invocable DF>
  requires transform_other<F, DF, const_reference>
  constexpr transform_type<F, const_reference> transform_or_else (F&& f, DF&& df) const
  noexcept(is_nothrow_invocable_v<F, const_reference> and is_nothrow_invocable_v<DF>) {
    if (*this) { return ::std::invoke(static_cast<F&&>(f), **this); }
    return this->transform_otherwise(static_cast<DF&&>(df));
  }

  template <class... Args> value_type& try_emplace (Args&&...) = delete;
  template <class... Args> value_type& emplace (Args&&...) = delete;

private:
  template <class F>
  static constexpr auto is_nothrow_otherwise = requires {
    requires ::std::is_nothrow_invocable_v<F>;
    requires ::std::is_void_v<::std::invoke_result_t<F>>
      or ::std::is_nothrow_constructible_v<optional, ::std::invoke_result_t<F>>;
  };

  template <invocable F>
  constexpr optional otherwise (F&& f) noexcept(is_nothrow_otherwise<F>) {
    if constexpr (not ::std::is_void_v<::std::invoke_result_t<F>>) {
      return optional { ::std::invoke(static_cast<F&&>(f)) };
    } else {
      ::std::invoke(static_cast<F&&>(f));
      return std::nullopt;
    }
  }

  template <invocable F, class U>
  constexpr transform_type<F, U> transform_otherwise (F&& f) noexcept(::std::is_nothrow_invocable_v<F>) {
    if constexpr (not ::std::is_void_v<::std::invoke_result_t<F>>) {
      return transform_type<F, U> { ::std::invoke(static_cast<F&&>(f)) };
    } else {
      ::std::invoke(static_cast<F&&>(f));
      return ::std::nullopt;
    }
  }

  storage_type storage { };
};

template <class T> optional (::std::reference_wrapper<T>) -> optional<T&>;
template <class T> optional (T) -> optional<T>;

// TODO: move these declarations below operator <=> once it is available.
extern template struct optional<unsigned long long>;
extern template struct optional<unsigned long>;
extern template struct optional<unsigned int>;
extern template struct optional<unsigned short>;

extern template struct optional<signed long long>;
extern template struct optional<signed long>;
extern template struct optional<signed int>;
extern template struct optional<signed short>;

extern template struct optional<unsigned char>;
extern template struct optional<signed char>;
extern template struct optional<char>;

extern template struct optional<double>;
extern template struct optional<float>;


//template <class T, std::three_way_comparable_with<T> U>
//constexpr std::compare_three_way_result_t<T, U>
//operator <=> (optional<T> const& lhs, optional<U> const& rhs) noexcept(noexcept(*lhs <=> *rhs)) {
//  if (lhs and rhs) { return *lhs <=> *rhs; }
//  return static_cast<bool>(lhs) <=> static_cast<bool>(rhs);
//}
//
//template <class T>
//constexpr std::strong_ordering operator <=> (optional<T> const& lhs, std::nullopt_t) noexcept {
//  return static_cast<bool>(lhs) <=> false;
//}
//
//template <class T, std::three_way_comparable_with<T> U>
//constexpr std::compare_three_way_result_t<T, U>
//operator <=> (optional<T> const& lhs, U const& rhs) noexcept(noexcept(*lhs <=> rhs)) {
//  return lhs ? *lhs <=> rhs : std::strong_ordering::less;
//}

// TODO: Delete these once <compare> and operator <=> are available
// This will let us delete about 162 lines of code. Which is always a good thing
template <class T, class U>
constexpr bool operator == (optional<T> const& lhs, optional<U> const& rhs) noexcept(noexcept(*lhs == *rhs)) {
  if (static_cast<bool>(lhs) != static_cast<bool>(rhs)) { return false; }
  if (not lhs and not rhs) { return true; }
  return *lhs == *rhs;
}

template <class T, class U>
constexpr bool operator != (optional<T> const& lhs, optional<U> const& rhs) noexcept(noexcept(*lhs != *rhs)) {
  if (static_cast<bool>(lhs) != static_cast<bool>(rhs)) { return true; }
  if (not lhs and not rhs) { return false; }
  return *lhs != *rhs;
}

template <class T, class U>
constexpr bool operator >= (optional<T> const& lhs, optional<U> const& rhs) noexcept(noexcept(*lhs >= *rhs)) {
  if (not rhs) { return true; }
  if (not lhs) { return false; }
  return *lhs >= *rhs;
}

template <class T, class U>
constexpr bool operator <= (optional<T> const& lhs, optional<U> const& rhs) noexcept(noexcept(*lhs <= *rhs)) {
  if (not lhs) { return true; }
  if (not rhs) { return false; }
  return *lhs <= *rhs;
}

template <class T, class U>
constexpr bool operator > (optional<T> const& lhs, optional<U> const& rhs) noexcept(noexcept(*lhs > *rhs)) {
  if (not lhs) { return false; }
  if (not rhs) { return true; }
  return *lhs > *rhs;
}

template <class T, class U>
constexpr bool operator < (optional<T> const& lhs, optional<U> const& rhs) noexcept(noexcept(*lhs < *rhs)) {
  if (not rhs) { return false; }
  if (not lhs) { return true; }
  return *lhs < *rhs;
}

template <class T>
constexpr bool operator == (optional<T> const& lhs, std::nullopt_t) noexcept {
  return not lhs;
}

template <class T>
constexpr bool operator != (optional<T> const& lhs, std::nullopt_t) noexcept {
  return static_cast<bool>(lhs);
}

template <class T>
constexpr bool operator >= (optional<T> const&, std::nullopt_t) noexcept {
  return true;
}

template <class T>
constexpr bool operator <= (optional<T> const& lhs, std::nullopt_t) noexcept {
  return not lhs;
}

template <class T>
constexpr bool operator > (optional<T> const& lhs, std::nullopt_t) noexcept {
  return static_cast<bool>(lhs);
}

template <class T>
constexpr bool operator < (optional<T> const&, std::nullopt_t) noexcept {
  return false;
}

template <class T>
constexpr bool operator == (std::nullopt_t, optional<T> const& rhs) noexcept {
  return not rhs;
}

template <class T>
constexpr bool operator != (std::nullopt_t, optional<T> const& rhs) noexcept {
  return static_cast<bool>(rhs);
}

template <class T>
constexpr bool operator >= (std::nullopt_t, optional<T> const& rhs) noexcept {
  return not rhs;
}

template <class T>
constexpr bool operator <= (std::nullopt_t, optional<T> const&) noexcept {
  return true;
}

template <class T>
constexpr bool operator > (std::nullopt_t, optional<T> const&) noexcept {
  return false;
}

template <class T>
constexpr bool operator < (std::nullopt_t, optional<T> const& rhs) noexcept {
  return static_cast<bool>(rhs);
}

template <class T, class U>
constexpr bool operator == (optional<T> const& lhs, U const& rhs) noexcept(noexcept(*lhs == rhs)) {
  return lhs ? *lhs == rhs : false;
}

template <class T, class U>
constexpr bool operator != (optional<T> const& lhs, U const& rhs) noexcept(noexcept(*lhs != rhs)) {
  return lhs ? *lhs != rhs : true;
}

template <class T, class U>
constexpr bool operator >= (optional<T> const& lhs, U const& rhs) noexcept(noexcept(*lhs >= rhs)) {
  return lhs ? *lhs >= rhs : false;
}

template <class T, class U>
constexpr bool operator <= (optional<T> const& lhs, U const& rhs) noexcept(noexcept(*lhs <= rhs)) {
  return lhs ? *lhs <= rhs : true;
}

template <class T, class U>
constexpr bool operator > (optional<T> const& lhs, U const& rhs) noexcept(noexcept(*lhs > rhs)) {
  return lhs ? *lhs > rhs : false;
}

template <class T, class U>
constexpr bool operator < (optional<T> const& lhs, U const& rhs) noexcept(noexcept(*lhs < rhs)) {
  return lhs ? *lhs < rhs : true;
}

template <class T, class U>
constexpr bool operator == (T const& lhs, optional<U> const& rhs) noexcept(noexcept(rhs == lhs)) {
  return rhs == lhs;
}

template <class T, class U>
constexpr bool operator != (T const& lhs, optional<U> const& rhs) noexcept(noexcept(rhs != lhs)) {
  return rhs != lhs;
}

template <class T, class U>
constexpr bool operator >= (T const& lhs, optional<U> const& rhs) noexcept(noexcept(lhs >= *rhs)) {
  return rhs ? lhs >= *rhs : true;
}

template <class T, class U>
constexpr bool operator <= (T const& lhs, optional<U> const& rhs) noexcept(noexcept(lhs <= *rhs)) {
  return rhs ? lhs <= *rhs : false;
}

template <class T, class U>
constexpr bool operator > (T const& lhs, optional<U> const& rhs) noexcept(noexcept(lhs > *rhs)) {
  return rhs ? lhs > *rhs : true;
}

template <class T, class U>
constexpr bool operator < (T const& lhs, optional<U> const& rhs) noexcept(noexcept(lhs < *rhs)) {
  return rhs ? lhs < *rhs : false;
}

} /* namespace apex */

#endif /* APEX_CORE_OPTIONAL_HPP */
