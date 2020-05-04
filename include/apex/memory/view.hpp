#ifndef APEX_MEMORY_VIEW_HPP
#define APEX_MEMORY_VIEW_HPP

namespace apex {
inline namespace v1 {

template <class T>
struct view_ptr {
  using element_type = T;
  using pointer = std::add_pointer_t<element_type>;

  constexpr view_ptr  () noexcept = default;

  //template <class T> constexpr view_ptr (view_ptr<T> that);
  constexpr view_ptr (std::nullptr_t) noexcept : view_ptr { } { };
  explicit view_ptr (pointer ptr) noexcept : ptr { ptr } { }

  view_ptr& operator = (std::nullptr_t) noexcept {
    this->reset();
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

private:
  pointer ptr { };
};

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
bool operator == (view_ptr<T> const& lhs, ::std::nullptr_t) noexcept {
  return lhs.get() == nullptr;
}

template <class T>
bool operator != (view_ptr<T> const& lhs, ::std::nullptr_t) noexcept {
  return lhs.get() != nullptr;
}

template <class T>
bool operator == (::std::nullptr_t, view_ptr<T> const& rhs) noexcept {
  return nullptr == rhs.get();
}

template <class T>
bool operator != (::std::nullptr_t, view_ptr<T> const& rhs) noexcept {
  return nullptr != rhs.get();
}

}} /* namespace apex::v1 */

#endif /* APEX_MEMORY_VIEW_HPP */
