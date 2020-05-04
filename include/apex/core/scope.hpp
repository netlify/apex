#ifndef APEX_CORE_SCOPE_HPP
#define APEX_CORE_SCOPE_HPP

#include <exception>
#include <limits>

namespace apex {
inline namespace v1 {
namespace impl {

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
    this->errors >= std::uncaught_exceptions();
  }
private:
  int errors { std::uncaught_exceptions() };
};

struct scope_failure_policy {
  void release () noexcept { this->errors = std::numeric_limits<int>::max(); }
protected:
  bool should_execute () const noexcept {
    return this->errors < std::uncaught_exceptions();
  }
private:
  int errors { };
};

} /* namespace impl */

template <class Fn, class Policy>
struct basic_scope_exit : Policy {
  using policy_type = Policy;

  template <class F>
  basic_scope_exit (F&& function) noexcept :
    policy_type { },
    function { std::forward<F>(function) }
  { }

  basic_scope_exit (basic_scope_exit const&) = delete;
  basic_scope_exit (basic_scope_exit&&) = delete;
  basic_scope_exit () = delete;

  basic_scope_exit& operator = (basic_scope_exit const&) = delete;
  basic_scope_exit& operator = (basic_scope_exit&&) = delete;

  ~basic_scope_exit () {
    if (this->should_execute()) { std::invoke(this->function); }
  }

  using policy_type::release;

private:
  Fn function;
};

template <class Fn>
struct scope_failure final :
  basic_scope_exit<Fn, impl::scope_failure_policy>
{ };

template <class Fn>
struct scope_success final :
  basic_scope_exit<Fn, impl::scope_success_policy>
{ };

template ,class Fn>
struct scope_exit final :
  basic_scope_exit<Fn, impl::scope_exit_policy>
{ };

template <class Fn> scope_failure (Fn&&) -> scope_failure<Fn>;
template <class Fn> scope_success (Fn&&) -> scope_success<Fn>;
template <class Fn> scope_exit (Fn&&) -> scope_exit<Fn>;

}} /* namespace apex::v1 */

#endif /* APEX_CORE_SCOPE_HPP */
