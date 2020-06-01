#ifndef APEX_MIXIN_SINGLETON_HPP
#define APEX_MIXIN_SINGLETON_HPP

#include <apex/core/concepts.hpp>

namespace apex {

template <class T> requires default_initializable
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
