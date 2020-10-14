#ifndef APEX_CORE_EXCEPTION_HPP
#define APEX_CORE_EXCEPTION_HPP

#include <apex/core/string.hpp>
#include <apex/core/source.hpp>
#include <exception>

namespace apex {

struct exception : ::std::exception {
  using ::std::exception::what;

  exception (apex::source_location=apex::source_location::current()) noexcept;
  exception () noexcept = delete;

  virtual ~exception () noexcept = default;

  char const* function_name () const noexcept;
  char const* file_name () const noexcept;
  size_t column () const noexcept;
  size_t line () const noexcept;

  apex::zstring_view function () const noexcept;
  apex::zstring_view file () const noexcept;

private:
  apex::source_location location;
};

struct bad_member_access : exception {
  using exception::exception;
};

template <class> struct bad_either_access;

template <>
struct bad_either_access<void> : bad_member_access {
  using bad_member_access::bad_member_access;
  virtual char const* what () const noexcept override final;
};

template <class T>
struct bad_either_access : bad_either_access<void> {
  using bad_either_access<void>::bad_either_access;
  using value_type = T;

  template <class U> requires constructible_from<value_type, U>
  constexpr explicit bad_either_access(U&& value, source_location location = source_location::current())
  noexcept(safely_constructible_from<value_type, U>) :
    bad_either_access<void>(location),
    value(static_cast<U&&>(value))
  { }

  constexpr decltype(auto) get () const&& noexcept { return static_cast<value_type const&&>(this->value); }
  constexpr decltype(auto) get () const& noexcept { return static_cast<value_type const&>(this->value); }
  constexpr decltype(auto) get () && noexcept { return static_cast<value_type&&>(this->value); }
  constexpr decltype(auto) get () & noexcept { return static_cast<value_type&>(this->value); }

private:
  value_type value;
};

template <class T>
struct bad_either_access<T&> : bad_either_access<void> {
  using bad_either_access<void>::bad_either_access;
  using value_type = T&;

  constexpr explicit bad_either_access (value_type value, source_location loc = source_location::current()) noexcept :
    bad_either_access<void>(loc),
    value(value)
  { }

  constexpr explicit bad_either_access (T&&) = delete;

  constexpr decltype(auto) get () const noexcept { return static_cast<T const&>(this->value); }
  constexpr decltype(auto) get () noexcept { return static_cast<T&>(this->value); }
private:
  value_type value;
};

template <class T> bad_either_access (T&&) -> bad_either_access<::std::remove_cvref_t<T>>;

struct bad_optional_access final : bad_member_access {
  using bad_member_access::bad_member_access;

  virtual char const* what () const noexcept override final;
};

} /* namespace apex */

#endif /* APEX_CORE_EXCEPTION_HPP */
