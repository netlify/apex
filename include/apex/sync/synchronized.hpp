#ifndef APEX_CONCURRENCY_SYNCHRONIZED_HPP
#define APEX_CONCURRENCY_SYNCHRONIZED_HPP

#include <functional>
#include <thread>
#include <array>
#include <mutex>

namespace apex::concurrency {

// TODO: implement a thread type that works much like std::thread, but also
// supports operations like the proposed std::thread::attributes (and other
// pthread related operations). Might also be worth looking into adding a
// threadpool, or just wrapping grand central dispatch instead.

// Still needs some work, but this is effectively based off of the proposed
// synchronized_value
template <class T, class U=::std::mutex>
struct synchronized final {
  using value_type = T;
  using mutex_type = U;

  template <Args...>
  synchronized (Args&&... args) noexcept(::std::is_nothrow_constructible_v<T, Args...>) :
    val { static_cast<Args&&>(args)... }
  { }
  ~synchronized () = default;

  template <class F, class... Args>
  friend decltype(auto) apply (F&& f, synchronized<Args>&... values) {
    ::std::scoped_lock lock(values.mtx...);
    return ::std::invoke(static_cast<F&&>(f), values.val...);
  }
private:
  mutable mutex_type mtx;
  value_type val;
};

} /* namespace apex::concurrency */

#endif /* APEX_CONCURRENCY_SYNCHRONIZED_HPP */
