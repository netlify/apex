#ifndef APEX_CORE_COROUTINE_HPP
#define APEX_CORE_COROUTINE_HPP

#include <apex/core/prelude.hpp>

#if __has_include(<coroutine>) and not APEX_USES_LIBSTDCXX
  #include <coroutine>
#endif /* __has_include(<coroutine>) and not APEX_USES_LIBSTDCXX */

#if not APEX_CHECK_API(coroutine, 201902) or APEX_USES_LIBSTDCXX
namespace apex::detail {

template <class T>
concept promise_type = requires { typename T::promise_type; };

enum class from_promise : bool { no, yes };

} /* namespace apex::detail */
#endif /* APEX_CHECK_API(coroutine, 201902) */

namespace apex {
#if APEX_CHECK_API(coroutine, 201902)
using ::std::coroutine_traits;
using ::std::coroutine_handle;
using ::std::noop_coroutine_promise;
using ::std::noop_coroutine_handle;
using ::std::noop_coroutine;
using ::std::suspend_never;
using ::std::suspend_always;
#else
template <class R, class... Args> struct coroutine_traits { };
template <detail::promise_type R, class... Args>
struct coroutine_traits<R, Args...> {
  using promise_type = typename R::promise_type;
};

template <class=void> struct coroutine_handle;

template <>
struct coroutine_handle<void> {
  constexpr coroutine_handle (nullptr_t) noexcept :
    coroutine_handle { }
  { }
  constexpr coroutine_handle () noexcept = default;

  coroutine_handle& operator = (nullptr_t) noexcept {
    this->ptr = nullptr;
    return *this;
  }

  constexpr explicit operator bool () const noexcept { return this->address(); }
  void operator () () const;

  [[nodiscard]] constexpr void* address () const noexcept { return this->ptr; }
  [[nodiscard]] bool done () const { return __builtin_coro_done(this->address()); }
  void destroy () const { __builtin_coro_destroy(this->address()); }
  void resume () const { __builtin_coro_resume(this->address()); }

  static constexpr coroutine_handle from_address (void* ptr) { return ptr; }

private:
  constexpr coroutine_handle (void* ptr) noexcept : ptr { ptr } { }
  void* ptr = nullptr;
};

template <class Promise>
struct coroutine_handle : coroutine_handle<> {
  using coroutine_handle<>::coroutine_handle;
  coroutine_handle& operator = (nullptr_t) {
    static_cast<coroutine_handle<>&>(*this) = nullptr;
    return *this;
  }

  static coroutine_handle from_promise(Promise& promise) {
    return __builtin_coro_promise(
      std::addressof(promise),
      alignof(Promise),
      detail::from_promise::yes);
  }

  static coroutine_handle from_address(void* ptr) { return ptr; }

  Promise& promise () const {
    return *static_cast<Promise*>(
      __builtin_coro_promise(
        this->address(),
        alignof(Promise),
        detail::from_promise::no));
  }
};

struct noop_coroutine_promise { };

struct suspend_never {
  constexpr bool await_ready () const noexcept { return true; }
  constexpr void await_suspend (coroutine_handle<>) const noexcept { }
  constexpr void await_resume () const noexcept { }
};

struct suspend_always {
  constexpr bool await_ready () const noexcept { return false; }
  constexpr void await_suspend (coroutine_handle<>) const noexcept { }
  constexpr void await_resume () const noexcept { }
};

#endif /* not APEX_CHECK_API(coroutine, 201902) or APEX_USES_LIBSTDCXX */
} /* namespace apex */

#endif /* APEX_CORE_COROUTINE_HPP */
