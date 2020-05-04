#ifndef APEX_MIXIN_SINGLETON_HPP
#define APEX_MIXIN_SINGLETON_HPP

namespace apex {
inline namespace v1 {

template <class T>
struct singleton {
  static_assert(std::is_default_constructible_v<T>);
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

}} /* namespace apex::v1 */

#endif /* APEX_MIXIN_SINGLETON_HPP */
