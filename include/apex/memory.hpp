#ifndef APEX_MEMORY_HPP
#define APEX_MEMORY_HPP

// This file contains several smart pointers and a reimplementation of
// std::unique_ptr to reduce overhead that currently exists in many
// implementations and cannot be resolved without changing the ABI

#include <memory>

namespace apex {
inline namespace v1 {

template <class> struct default_delete;
template <class> struct retain_traits;

struct retain_t { retain_t () noexcept = default; };
struct adopt_t { adopt_t () noexcept = default; };

inline constexpr retain_t retain { };
inline constexpr adopt_t adopt { };

template <class T, class D> struct unique_ptr; // std::unique_ptr has ABI issues
// P0468
template <class T, class R=retain_traits<T>>
struct retain_ptr {
  // TODO: Set these correctly. Swap them up
  using element_type = T;
  using traits_type = R;
  using pointer = std::add_pointer_t<element_type>;

  using reset_action = traits_type::reset_action;

  // TODO: correct noexcept capabilities
  retain_ptr (pointer ptr, retain_t) noexcept(false) :
    retain_ptr { ptr, adopt }
  { if (*this) { traits_type::increment(this->get()); } }

  retain_ptr (pointer ptr, adopt_t) noexcept :
    ptr { ptr }
  { }

  explicit retain_ptr (pointer ptr) noexcept :
    retain_ptr { ptr, reset_action { } }
  { }

  retain_ptr (nullptr_t) noexcept : retain_ptr { } { }

  retain_ptr (retain_ptr const& that) noexcept :
    retain_ptr { that.ptr, retain }
  { }

  retain_ptr (retain_ptr&& that) noexcept :
    ptr { that.release() }
  { }

  retain_ptr () noexcept = default;
  ~retain_ptr () noexcept {
    if (*this) { traits_type::decrement(this->get()); }
  }

  retain_ptr& operator = (retain_ptr const& that) noexcept {
    retain_ptr(that).swap(*this);
    return *this;
  }

  retain_ptr& operator = (retain_ptr&& that) noexcept {
    retain_ptr(std::move(that)).swap(*this);
    return *this;
  }

  retain_ptr& operator = (nullptr_t) noexcept {
    this->reset();
    return *this;
  }

  void swap (retain_ptr& that) noexcept {
    using std::swap;
    swap(this->ptr, that.ptr);
  }

  explicit operator bool () const noexcept { return this->get(); }
  decltype(auto) operator * () const noexcept { return *this->get(); }
  pointer operator -> () const noexcept { return this->get(); }

  pointer get () const noexcept { return this->ptr; }

  [[nodiscard]] pointer release () noexcept {
    return std::exchange(this->ptr, pointer { });
  }

  void reset (pointer ptr, retain_t) noexcept {
    *this = retain_ptr(ptr, retain);
  }

  void reset (pointer ptr, adopt_t) noexcept {
    *this = retain_ptr(ptr, adopt);
  }

  void reset (pointer ptr) noexcept {
    this->reset(ptr, reset_action { });
  }

private:
  pointer ptr { };
};

//template <class T> retain_ptr (std::unique_ptr<T>&) -> retain_ptr<T>;

// This is polymorphic_value, but renamed (it's a bit closer to Box<T> from Rust)
// TODO: maybe just name this box<T>?
template <class T> struct value_ptr;

// this is observer_ptr, but renamed
// TODO: maybe reconsider the name? survey is... odd. view_ptr feels weird,
// as well.
template <class T>
struct survey_ptr {
  using element_type = T;
  using pointer = std::add_pointer_t<element_type>;

  constexpr survey_ptr  () noexcept = default;

  //template <class T> constexpr survey_ptr (survey_ptr<T> that);
  constexpr survey_ptr (std::nullptr_t) noexcept : survey_ptr { } { };
  explicit survey_ptr (pointer ptr) noexcept : ptr { ptr } { }

  survey_ptr& operator = (std::nullptr_t) noexcept {
    this->reset();
    return *this;
  }

  explicit operator pointer () const noexcept { return this->ptr; }
  explicit operator bool () const noexcept { return this->ptr; }

  decltype(auto) operator * () const noexcept { return *this->ptr; }
  auto operator -> () const noexcept { return this->ptr; }

  void swap (survey_ptr& that) noexcept {
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

template <class T, class R>
void swap (retain_ptr<T, R>& lhs, retain_ptr<T, R>& rhs) noexcept {
  lhs.swap(rhs);
}

template <class T>
void swap (survey_ptr<T>& lhs, survey_ptr<T>& rhs) noexcept { lhs.swap(rhs); }

template <class T, class U>
bool operator == (survey_ptr<T> const& lhs, survey_ptr<U> const& rhs) noexcept {
  return lhs.get() == rhs.get();
}

template <class T, class U>
bool operator != (survey_ptr<T> const& lhs, survey_ptr<U> const& rhs) noexcept {
  return lhs.get() != rhs.get();
}

template <class T, class U>
bool operator >= (survey_ptr<T> const& lhs, survey_ptr<U> const& rhs) noexcept {
  return lhs.get() >= rhs.get();
}

template <class T, class U>
bool operator <= (survey_ptr<T> const& lhs, survey_ptr<U> const& rhs) noexcept {
  return lhs.get() <= rhs.get();
}

template <class T, class U>
bool operator > (survey_ptr<T> const& lhs, survey_ptr<U> const& rhs) noexcept {
  return lhs.get() > rhs.get();
}

template <class T, class U>
bool operator < (survey_ptr<T> const& lhs, survey_ptr<U> const& rhs) noexcept {
  return lhs.get() < rhs.get();
}

template <class T>
bool operator == (survey_ptr<T> const& lhs, ::std::nullptr_t) noexcept {
  return lhs.get() == nullptr;
}

template <class T>
bool operator != (survey_ptr<T> const& lhs, ::std::nullptr_t) noexcept {
  return lhs.get() != nullptr;
}

template <class T>
bool operator == (::std::nullptr_t, survey_ptr<T> const& rhs) noexcept {
  return nullptr == rhs.get();
}

template <class T>
bool operator != (::std::nullptr_t, survey_ptr<T> const& rhs) noexcept {
  return nullptr != rhs.get();
}

/* shim definitions */
#if __cpp_lib_to_address >= 201711L
using ::std::to_address;
#else
template <class T>
constexpr auto to_address (T const& p) noexcept { return p.operator->(); }

template <class T>
constexpr T* to_address (T* p) noexcept { 
  static_assert(not std::is_function_v<T>);
  return p;
}
#endif /* __cpp_lib_to_address >= 201711L */

// This can't be constexpr under C++17 for a variety of reasons. However we
// don't really use it in such a manner, so it's fine for what we're doing
// here.
#if __cpp_lib_constexpr_dynamic_alloc >= 201907
using ::std::construct_at;
using ::std::destroy_at;
#else
template <class T>
void destroy_at (T* p) noexcept(std::is_nothrow_destructible_v<T>) {
  if constexpr (std::is_array_v<T>) {
    for (auto& elem : *p) { destroy_at(std::addressof(elem)); }
  } else { p->~T(); }
}

template <class T, class... Args, std::enable_if_t<std::is_constructible_v<T, Args...>, int> = __LINE__>
auto construct_at (T* location, Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>) {
  if constexpr (std::is_aggregate_v<T>) {
    return ::new (static_cast<void*>(location)) T { std::forward<Args>(args)... };
  } else {
    return ::new (static_cast<void*>(location)) T(std::forward<Args>(args)...);
  }
}
#endif /* __cpp_lib_constexpr_dynamic_alloc */

}} /* namespace apex::v1 */

#endif /* APEX_MEMORY_HPP */
