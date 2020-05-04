#ifndef APEX_CORE_FUNCTIONAL_HPP
#define APEX_CORE_FUNCTIONAL_HPP

#include <apex/core/traits.hpp>

#include <functional>
#include <tuple>

namespace apex {
inline namespace v1 {

// This does not follow the specification "to the letter", but we'll rarely run
// into the issue that our break performs. By the time this function is
// available everywhere we won't be using our implementation anyhow.
// Doing it "the correct way" will increase the amount of work needed.
template <class F, class... Args>
constexpr auto bind_front (F&& f, Args&&... args) {
  static_assert((std::is_move_constructible_v<std::decay_t<Args>, Args> and ...));
  static_assert((std::is_constructible_v<std::decay_t<Args>, Args> and ...));
  static_assert(std::is_move_constructible_v<std::decay_t<F>>);
  static_assert(std::is_constructible_v<std::decay_t<F>>);
  return [
    f = std::forward<F>(f)
    bound = std::tuple<std::decay_t<Args>...>(std::forward<Args>(args)...)
  ] (auto&&... args) {
    return std::apply(
      std::forward<F>(f),
      std::tuple_cat(
        bound,
        std::forward_as_tuple(std::forward<decltype(args)>(args)...)
      )
    );
  };
}

template <class> struct function_ref;

template <class R, class... Args>
struct function_ref<R(Args...)> {
  using callback_type = std::add_pointer_t<R(void*, Args...)>;

  template <class F>
  constexpr static bool is_constructible = std::conjunction_v<
    std::negation<std::is_same<remove_cvref_t<F>, function_ref>>,
    std::is_invocable_r<R, F&&, Args...>
  >;

  template <class F, class=std::enable_if_t<is_constructible<F>>>
  constexpr function_ref (F&& f) noexcept :
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
    this->callback = [] (void* ptr, Args... args) -> R {
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

template <class R, class... Args>
struct function_ref<R(Args...) noexcept> {
  using callback_type = std::add_pointer_t<R(void*, Args...) noexcept>;

  template <class F>
  constexpr static bool is_constructible = std::conjunction_v<
    std::negation<std::is_same<remove_cvref_t<F>, function_ref>>,
    std::is_nothrow_invocable_r<R, F&&, Args...>
  >;

  template <class F, class=std::enable_if_t<is_constructible<F>>>
  constexpr function_ref (F&& f) noexcept :
    callback {
      [] (void* ptr, Args... args) noexcept -> R {
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
    this->callback = [] (void* ptr, Args... args) noexcept -> R{
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

  R operator () (Args... args) const noexcept {
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

template <class R, class... Args>
function_ref (R (*)(Args...) noexcept) -> function_ref<R(Args...) noexcept>;

}} /* namespace apex::v1 */



#endif /* APEX_CORE_FUNCTIONAL_HPP */
