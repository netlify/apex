#ifndef APEX_CORE_SCOPE_HPP
#define APEX_CORE_SCOPE_HPP

#include <apex/core/concepts.hpp>

#include <functional>
#include <exception>
#include <limits>

namespace apex::impl {

struct scope_exit_policy {
  void release () noexcept { this->execute = false; }
protected:
  constexpr bool should_execute () const noexcept { return this->execute; }
private:
  bool execute { true };
};

struct scope_success_policy {
  void release () noexcept { this->errors = -1; }
protected:
  bool should_execute () const noexcept {
    return this->errors >= std::uncaught_exceptions();
  }
private:
  int errors { std::uncaught_exceptions() };
};

struct scope_failure_policy {
  void release () noexcept { this->errors = ::std::numeric_limits<int>::max(); }
protected:
  bool should_execute () const noexcept {
    return this->errors < ::std::uncaught_exceptions();
  }
private:
  int errors { };
};

} /* namespace apex::impl */

namespace apex {

template <class Fn, class Policy>
struct basic_scope_exit : Policy {
  using policy_type = Policy;

  template <constructible_from<Fn> F>
  basic_scope_exit (F&& function) noexcept :
    policy_type { },
    function { static_cast<F&&>(function) }
  { }

  basic_scope_exit (basic_scope_exit const&) = delete;
  basic_scope_exit (basic_scope_exit&&) = delete;
  basic_scope_exit () = delete;

  basic_scope_exit& operator = (basic_scope_exit const&) = delete;
  basic_scope_exit& operator = (basic_scope_exit&&) = delete;

  ~basic_scope_exit () {
    if (this->should_execute()) { ::std::invoke(this->function); }
  }

  using policy_type::release;

private:
  Fn function;
};

template <class Fn>
struct [[nodiscard]] scope_failure final :
  basic_scope_exit<Fn, impl::scope_failure_policy>
{ };

template <class Fn>
struct [[nodiscard]] scope_success final :
  basic_scope_exit<Fn, impl::scope_success_policy>
{ };

template <class Fn>
struct [[nodiscard]] scope_exit final :
  basic_scope_exit<Fn, impl::scope_exit_policy>
{ };

template <class Fn> scope_failure (Fn&&) -> scope_failure<Fn>;
template <class Fn> scope_success (Fn&&) -> scope_success<Fn>;
template <class Fn> scope_exit (Fn&&) -> scope_exit<Fn>;

} /* namespace apex */

#endif /* APEX_CORE_SCOPE_HPP */
