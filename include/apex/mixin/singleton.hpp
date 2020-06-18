#ifndef APEX_MIXIN_SINGLETON_HPP
#define APEX_MIXIN_SINGLETON_HPP

#include <apex/core/concepts.hpp>

// This requires more work. The singleton<T> type should, honestly, be hidden
// behind a detail:: namespace. Additionally, this isn't really a mixin, is it?

namespace apex {

template <default_initializable T>
struct singleton {
  singleton (singleton const&) = delete;
  singleton () = delete;
  ~singleton () = delete;

  singleton& operator = (singleton const&) = delete;

  static T& global () noexcept {
    static T instance;
    return instance;
  };

  static T& local () noexcept {
    static thread_local T instance;
    return instance;
  }
};

template <class T> inline auto& global = singleton<T>::global();
template <class T> inline auto& local = singleton<T>::local();

template <class T> inline auto const& global<T const> = singleton<T>::global();
template <class T> inline auto const& local<T const> = singleton<T>::local();

} /* namespace apex */

#endif /* APEX_MIXIN_SINGLETON_HPP */
