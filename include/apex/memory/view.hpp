#ifndef APEX_MEMORY_VIEW_HPP
#define APEX_MEMORY_VIEW_HPP

#include <apex/core/prelude.hpp>
#include <apex/core/utility.hpp>

#if __has_include(<compare>)
  #include <compare>
#endif /* __has_include(<compare>) */


namespace apex {
inline namespace v1 {

template <class T>
struct view_ptr {
  using element_type = T;
  using pointer = add_pointer_t<element_type>;

  constexpr view_ptr (view_ptr const&) noexcept = default;
  constexpr view_ptr () noexcept = default;

  template <distinct_from<T> U>
  constexpr view_ptr (view_ptr<U> that) noexcept :
    view_ptr { that.get() }
  { }
  constexpr explicit view_ptr (pointer ptr) noexcept : ptr { ptr } { }
  constexpr view_ptr (nullptr_t) noexcept : view_ptr { } { };

  view_ptr& operator = (view_ptr const&) noexcept = default;

  view_ptr& operator = (nullptr_t) noexcept {
    this->reset();
    return *this;
  }

  view_ptr& operator = (pointer ptr) noexcept {
    this->reset(ptr);
    return *this;
  }

  explicit operator pointer () const noexcept { return this->ptr; }
  explicit operator bool () const noexcept { return this->ptr; }

  decltype(auto) operator * () const noexcept { return *this->ptr; }
  auto operator -> () const noexcept { return this->ptr; }

  void swap (view_ptr& that) noexcept {
    using std::swap;
    swap(this->ptr, that.ptr);
  }

  [[nodiscard]] pointer release () noexcept {
    return std::exchange(this->ptr, nullptr);
  }

  void reset (pointer ptr = nullptr) noexcept { this->ptr = ptr; }
  auto get () const noexcept { return this->ptr; }

  #if APEX_CHECK_API(three_way_comparison, 201907) and not APEX_USES_LIBSTDCXX
  friend std::strong_ordering operator <=> (view_ptr const&, view_ptr const&) noexcept = default;
  friend std::strong_equality operator <=> (view_ptr const&, nullptr_t) noexcept = default;
  friend std::strong_equality operator <=> (nullptr_t, view_ptr const&) noexcept = default;
  #endif /* APEX_CHECK_API(three_way_comparison, 201907) */

private:
  pointer ptr { };
};

template <class T> view_ptr(T*) -> view_ptr<T>;

template <class T>
void swap (view_ptr<T>& lhs, view_ptr<T>& rhs) noexcept { lhs.swap(rhs); }

template <class T, class U>
bool operator == (view_ptr<T> const& lhs, view_ptr<U> const& rhs) noexcept {
  return lhs.get() == rhs.get();
}

template <class T, class U>
bool operator != (view_ptr<T> const& lhs, view_ptr<U> const& rhs) noexcept {
  return lhs.get() != rhs.get();
}

template <class T, class U>
bool operator >= (view_ptr<T> const& lhs, view_ptr<U> const& rhs) noexcept {
  return lhs.get() >= rhs.get();
}

template <class T, class U>
bool operator <= (view_ptr<T> const& lhs, view_ptr<U> const& rhs) noexcept {
  return lhs.get() <= rhs.get();
}

template <class T, class U>
bool operator > (view_ptr<T> const& lhs, view_ptr<U> const& rhs) noexcept {
  return lhs.get() > rhs.get();
}

template <class T, class U>
bool operator < (view_ptr<T> const& lhs, view_ptr<U> const& rhs) noexcept {
  return lhs.get() < rhs.get();
}

template <class T>
bool operator == (view_ptr<T> const& lhs, nullptr_t) noexcept {
  return lhs.get() == nullptr;
}

template <class T>
bool operator != (view_ptr<T> const& lhs, nullptr_t) noexcept {
  return lhs.get() != nullptr;
}

template <class T>
bool operator == (nullptr_t, view_ptr<T> const& rhs) noexcept {
  return nullptr == rhs.get();
}

template <class T>
bool operator != (nullptr_t, view_ptr<T> const& rhs) noexcept {
  return nullptr != rhs.get();
}

}} /* namespace apex::v1 */

#endif /* APEX_MEMORY_VIEW_HPP */
