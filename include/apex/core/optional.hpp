#ifndef APEX_CORE_OPTIONAL_HPP
#define APEX_CORE_OPTIONAL_HPP

#include <apex/detail/core/optional.hpp>

// We implement our own optional because std::optional
// 1) cannot be specialized
// 2) Does not support references due to a lot of drama.
// 3) Is not as conditionally noexcept as it can be
// 4) Does not (currently) use concepts, and is thus under-constrained

// TODO:
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
template <class T>
struct optional final : private detail::optional::base<T> {
  static_assert(not ::std::is_rvalue_reference_v<T>);
  static_assert(not same_as<::std::nullopt_t, T>);

  using value_type = T;
  using base_type = detail::optional::base<T>;

  using base_type::base_type;

  using base_type::operator bool;
  using base_type::operator *;

  using base_type::try_emplace;
  using base_type::emplace;
  using base_type::reset;

  using base_type::has_value;
  using base_type::value_or;
  using base_type::value;

  using base_type::transform_or;
  using base_type::transform;
  using base_type::and_then;

  template <different_from<optional> U=value_type> requires constructible_from<value_type, U>
  constexpr explicit(not convertible_to<U, value_type>) optional (U&& value)
    noexcept(safely_constructible_from<value_type, U>) :
    optional { ::std::in_place, static_cast<U&&>(value) }
  { }

  constexpr optional (::std::nullopt_t) noexcept :
    optional { }
  { }

  template <class U=value_type>
  requires different_from<optional, U>
  and constructible_from<value_type, U>
  and assignable_from<value_type&, U>
  and ::std::is_object_v<value_type>
  optional& operator = (U&& value)
  noexcept(safely_constructible_from<value_type, U> and safely_assignable_from<value_type, U>) {
    if (*this) { **this = static_cast<U&&>(value); }
    else { this->construct(static_cast<U&&>(value)); }
    return *this;
  }

  optional& operator = (::std::nullopt_t) noexcept {
    this->reset();
    return *this;
  }

  friend void swap (optional& lhs, optional& rhs) noexcept(noexcept(apex::ranges::swap(*lhs, *rhs))) {
    if constexpr (::std::is_lvalue_reference_v<value_type>) {
      apex::ranges::swap(lhs.storage, rhs.storage);
    } else {
      if (not lhs and not rhs) { return; }
      if (lhs and rhs) { return apex::ranges::swap(*lhs, *rhs); }
      auto& uninitialized = lhs ? rhs : lhs;
      auto& initialized = lhs ? lhs : rhs;
      uninitialized = static_cast<optional&&>(lhs);
      initialized = ::std::nullopt;
    }
  }

  auto operator -> () const noexcept { return ::std::addressof(**this); }
  auto operator -> () noexcept { return ::std::addressof(**this); }

  constexpr optional or_else (invocable auto&& f) const&& noexcept(safely_invocable<decltype(f)>) {
    if (*this) {
      if constexpr (::std::is_lvalue_reference_v<value_type>) { return *this; }
      else { return static_cast<optional const&&>(*this); }
    }
    return this->otherwise(static_cast<decltype(f)>(f));
  }

  constexpr optional or_else (invocable auto&& f) const& noexcept(safely_invocable<decltype(f)>) {
    if (*this) { return *this; }
    return this->otherwise(static_cast<decltype(f)>(f));
  }

  constexpr optional or_else (invocable auto&& f) && noexcept(safely_invocable<decltype(f)>) {
    if (*this) {
      if constexpr (::std::is_lvalue_reference_v<value_type>) { return *this; }
      else { return static_cast<optional&&>(*this); }
    }
    return this->otherwise(static_cast<decltype(f)>(f));
  }

  constexpr optional or_else (invocable auto&& f) & noexcept(safely_invocable<decltype(f)>) {
    if (*this) { return *this; }
    return this->otherwise(static_cast<decltype(f)>(f));
  }

  constexpr auto transform_or_else (invocable auto&& df, invocable<value_type const&&> auto&& f) const&&
  noexcept(safely_invocable<decltype(df)> and safely_invocable<decltype(f), value_type const&&>) {
    if (not *this) { return this->otherwise(static_cast<decltype(df)>(df)); }
    return this->apply(static_cast<decltype(f)>(f));
  }

  constexpr auto transform_or_else (invocable auto&& df, invocable<value_type const&> auto&& f) const&
  noexcept(safely_invocable<decltype(df)> and safely_invocable<decltype(f), value_type const&>) {
    if (not *this) { return this->otherwise(static_cast<decltype(df)>(df)); }
    return this->apply(static_cast<decltype(f)>(f));
  }

  constexpr auto transform_or_else (invocable auto&& df, invocable<value_type&&> auto&& f) &&
  noexcept(safely_invocable<decltype(df)> and safely_invocable<decltype(f), value_type&&>) {
    if (not *this) { return this->otherwise(static_cast<decltype(df)>(df)); }
    return this->apply(static_cast<decltype(f)>(f));
  }

  constexpr auto transform_or_else (invocable auto&& df, invocable<value_type&> auto&& f) &
  noexcept(safely_invocable<decltype(df)> and safely_invocable<decltype(f), value_type&>) {
    if (not *this) { return this->otherwise(static_cast<decltype(df)>(df)); }
    return this->apply(static_cast<decltype(f)>(f));
  }

private:
  template <invocable F>
  constexpr optional otherwise (F&& f) const noexcept {
    if constexpr (not ::std::is_void_v<::std::invoke_result_t<F>>) {
      return optional { ::std::invoke(static_cast<F&&>(f)) };
    } else {
      ::std::invoke(static_cast<F&&>(f));
      return ::std::nullopt;
    }
  }
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
