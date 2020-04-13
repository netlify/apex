#ifndef APEX_FUNCTIONAL_HPP
#define APEX_FUNCTIONAL_HPP

#include <functional>

namespace apex {
inline namespace v1 {

// TODO: implement~
template <class F, class... Args>
constexpr auto bind_front (F&& f, Args&&... args) = delete;

template <class> struct function_ref;

template <class R, class... Args>
struct function_ref<R(Args...)> {
  using callback_type = std::add_pointer_t<R(void*, Args...)>;

  template <class F>
  constexpr static bool is_constructible = std::conjunction_v<
    std::negation<std::is_same<remove_cvref_t<F>, function_ref>>,
    std::is_invocable_r<R, F&&, Args...>
  >;

  // TODO: consider allowing F's return value to be implicitly *convertible* to R
  template <
    class F,
    class=std::enable_if_t<is_constructible<F>>
  > constexpr function_ref (F&& f) noexcept :
    callback {
      [](void* ptr, Args... args) -> R {
        return std::invoke(
          *reinterpret_cast<std::add_pointer_t<F>>(ptr),
          std::forward<Args>(args)...);
      }
    },
    ptr { std::addressof(f) }
  { }

  constexpr function_ref (function_ref const&) noexcept = default;
  constexpr function_ref () noexcept = delete;

  constexpr function_ref& operator = (function_ref const&) noexcept = default;

  template <class F, class=std::enable_if_t<is_constructible<F>>
  constexpr function_ref& operator = (F&& f) noexcept {
    this->callback = [] (void* ptr, Args... args) {
      return std::invoke(
        *reinterpret_cast<std::add_pointer_t<F>>(ptr),
        std::forward<Args>(args)...);
    };
    this->ptr = std::addressof(f);
    return *this;
  }

  constexpr void swap (function_ref& that) noexcept {
    using std::swap;
    swap(this->callback, that.callback);
    swap(this->ptr, that.ptr);
  }

  R operator () (Args... args) const {
    return std::invoke(callback, this->ptr, std::forward<Args>(args)...);
  }

private:
  callback_type callback { };
  void* ptr { };
};

template <class F>
constexpr void swap (function_ref<F>& lhs, function_ref<F>& rhs) noexcept {
  lhs.swap(rhs);
}

template <class R, class... Args>
function_ref (R (*)(Args...)) -> function_ref<R(Args...)>;

}} /* namespace apex::v1 */

#endif /* APEX_FUNCTIONAL_HPP */
