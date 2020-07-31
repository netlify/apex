#ifndef APEX_CORE_FUNCTIONAL_HPP
#define APEX_CORE_FUNCTIONAL_HPP

#include <apex/core/prelude.hpp>
#include <apex/core/traits.hpp>

// IWYU pragma: begin_exports
#include <functional>
#include <tuple>
// IWYU pragma: end_exports

namespace apex::impl {

template <class F, class... Ts>
struct bind_front final {

  template <class T> using const_lvalue_ref = std::add_lvalue_reference_t<std::add_const_t<T>>;
  template <class T> using const_rvalue_ref = std::add_rvalue_reference_t<std::add_const_t<T>>;

  template <template <class> class M, class... Args>
  static constexpr bool nothrow = std::is_nothrow_invocable_v<
    M<std::decay_t<F>>,
    M<std::decay_t<Ts>>...,
    Args...
  >;

  constexpr explicit bind_front (F&& f, Ts&&... args) :
    bound {std::forward<Ts>(args)...},
    function { std::forward<F>(f) }
  { }

  template <class... Args>
  constexpr decltype(auto) operator () (Args&&... args) & noexcept(nothrow<std::add_lvalue_reference_t, Args...>) {
    return std::apply(
      this->function,
      std::tuple_cat(
        this->bound,
        std::forward_as_tuple(static_cast<Args&&>(args)...)));
  }

  template <class... Args>
  constexpr decltype(auto) operator () (Args&&... args) const& noexcept(nothrow<const_lvalue_ref, Args...>) {
    return std::apply(
      this->function,
      std::tuple_cat(
        this->bound,
        std::forward_as_tuple(std::forward<Args>(args)...)));
  }

  template <class... Args>
  constexpr decltype(auto) operator () (Args&&... args) && noexcept(nothrow<std::add_rvalue_reference_t, Args...>) {
    return std::apply(
      std::move(this->function),
      std::tuple_cat(
        std::move(this->bound),
        std::forward_as_tuple(std::forward<Args>(args)...)));
  }

  template <class... Args>
  constexpr decltype(auto) operator () (Args&&... args) const&& noexcept(nothrow<const_rvalue_ref, Args...>) {
    return std::apply(
      std::move(this->function),
      std::tuple_cat(
        std::move(this->bound),
        std::forward_as_tuple(std::forward<Args>(args)...)));
  }

private:
  // TODO: use an apex::compressed<T, U> instead~
  // pair is not constexpr in C++17, so we can't make this entirely constexpr
  std::tuple<std::decay_t<Ts>...> bound;
  std::decay_t<F> function;
};

} /* namespace apex::impl */

namespace apex {

template <class F, class... Args>
constexpr auto bind_front (F&& f, Args&&... args) {
  static_assert((std::is_move_constructible_v<std::decay_t<Args>> and ...));
  static_assert((std::is_constructible_v<std::decay_t<Args>, Args> and ...));
  static_assert(std::is_move_constructible_v<std::decay_t<F>>);
  static_assert(std::is_constructible_v<std::decay_t<F>, F>);
  return apex::impl::bind_front<F, Args...>(std::forward<F>(f), std::forward<Args>(args)...);
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

  template <class F, class=std::enable_if_t<is_constructible<F>>>
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

  template <class F, class=std::enable_if_t<is_constructible<F>>>
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
    return ::std::invoke(callback, this->ptr, static_cast<Args&&>(args)...);
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

} /* namespace apex */

#endif /* APEX_CORE_FUNCTIONAL_HPP */
