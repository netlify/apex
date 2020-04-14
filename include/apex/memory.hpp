#ifndef APEX_MEMORY_HPP
#define APEX_MEMORY_HPP

// This file contains several smart pointers and a reimplementation of
// std::unique_ptr to reduce overhead that currently exists in many
// implementations and cannot be resolved without changing the ABI

#include <apex/traits.hpp>
#include <apex/types.hpp>
#include <memory>
#include <atomic>

namespace apex {
inline namespace v1 {
namespace impl {

template <class T, class P>
using has_use_count = decltype(T::use_count(std::declval<P>()));

template <class T>
using has_reset_action = typename T::reset_action;

template <class T>
struct stateful_deleter {
  T instance;
};

} /* namespace impl */

template <class T>
struct default_delete {
  void operator () (T* pointer) const noexcept { delete pointer; }
};

template <class T>
struct default_delete<T[]> {
  void operator () (T* pointer) const noexcept { delete[] pointer; }
};

template <class> struct retain_traits;

template <class T>
struct atomic_reference_count {
  template <class> friend class retain_traits;
protected:
  atomic_reference_count () = default;
private:
  std::atomic<long> count { 1 };
};

template <class T>
struct reference_count {
  template <class> friend class retain_traits;
protected:
  reference_count () = default;
private:
  long count { 1 };
};

struct retain_t { retain_t () noexcept = default; };
struct adopt_t { adopt_t () noexcept = default; };

inline constexpr retain_t retain { };
inline constexpr adopt_t adopt { };

template <class T>
struct retain_traits final {
  template <class U>
  using enable_if_base = std::enable_if_t<std::is_base_of_v<U, T>>;

  template <class U, class=enable_if_base<U>>
  static void increment (atomic_reference_count<U>* ptr) noexcept {
    ptr->count.fetch_add(1, std::memory_order_relaxed);
  }

  template <class U, class=enable_if_base<U>>
  static void decrement (atomic_reference_count<U>* ptr) noexcept {
    ptr->count.fetch_sub(1, std::memory_order_acq_rel);
    if (not use_count(ptr)) { delete static_cast<T*>(ptr); }
  }

  template <class U, class=enable_if_base<U>>
  static long use_count (atomic_reference_count<U>* ptr) noexcept {
    return ptr->count.load(std::memory_order_relaxed);
  }

  template <class U, class=enable_if_base<U>>
  static void increment (reference_count<U>* ptr) noexcept {
    ++ptr->count;
  }

  template <class U, class=enable_if_base<U>>
  static void decrement (reference_count<U>* ptr) noexcept {
    --ptr->count;
    if (not use_count(ptr)) { delete static_cast<T*>(ptr); }
  }

  template <class U, class=enable_if_base<U>>
  static long use_count (reference_count<U>* ptr) noexcept {
    return ptr->count;
  }
};

// std::unique_ptr has ABI issues. The point of this recreation is to
// allow it to be trivially_copyable. While this isn't "the best" argument,
// what it means is that a `sizeof(unique_ptr<int>) == sizeof(int*)`, which
// is not currently the case on all implementations.
//template <class T, class D=default_delete<T>>
//struct unique_ptr : std::conditional<
//  std::is_final_v<D>,
//  impl::stateful_deleter<D>,
//  D
//> {
//
//  explicit unique_ptr (pointer ptr) noexcept :
//    ptr { std::move(ptr) }
//  { }
//
//  unique_ptr (nullptr_t) noexcept : unique_ptr { } { }
//
//  unique_ptr (unique_ptr const&) = delete;
//  unique_ptr (unique_ptr&& that) noexcept : ptr { that.release() } { }
//  unique_ptr () noexcept = default;
//  ~unique_ptr () noexcept {
//    if (*this) { this->get_deleter()(this->get()); }
//  }
//
//  template <class U, class E>
//  unique_ptr& operator = (unique_ptr&& that) noexcept;
//  unique_ptr& operator = (unique_ptr&& that) noexcept;
//  unique_ptr& operator = (nullptr_t) noexcept;
//
//  void swap (unique_ptr& that) noexcept {
//    using std::swap;
//    swap(this->ptr, that.ptr);
//  }
//
//  explicit operator bool () const noexcept { return this->get(); }
//  decltype(auto) operator * () const noexcept { return *this->get(); }
//  pointer operator -> () const noexcept { return this->get(); }
//
//  pointer get () const noexcept { return this->ptr; }
//
//  D& get_deleter () noexcept {
//    if constexpr (std::is_base_of_v<D, unique_ptr>) { return *this; }
//    else { return impl::stateful_deleter<D>::get_deleter(); }
//  }
//
//  D const& get_deleter () const noexcept {
//    if constexpr (std::is_base_of_v<D, unique_ptr>) { return *this; }
//    else { return impl::stateful_deleter<D>::get_deleter(); }
//  }
//
//  [[nodiscard]] auto release () noexcept {
//    return std::exchange(this->ptr, pointer { });
//  }
//
//  void reset (pointer ptr = pointer { }) noexcept {
//    this->get_deleter()(std::exchange(this->ptr, ptr));
//  }
//private:
//  pointer ptr { };
//};

// P0468, with more explicit noexcept expressions
template <class T, class R=retain_traits<T>>
struct retain_ptr {
  using element_type = T;
  using traits_type = R;

  using pointer = detected_or_t<
    std::add_pointer_t<element_type>,
    detect::pointer,
    traits_type
  >;

  using reset_action = detected_or_t<
    adopt_t,
    impl::has_reset_action,
    traits_type
  >;

  static_assert(std::disjunction_v<
    std::is_same<reset_action, retain_t>,
    std::is_same<reset_action, adopt_t>
  >);

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
// TODO: maybe just name this to box<T>?
template <class T> struct value_ptr;

// observer_ptr, but with a better name.
template <class T>
struct spectate_ptr {
  using element_type = T;
  using pointer = std::add_pointer_t<element_type>;

  constexpr spectate_ptr  () noexcept = default;

  //template <class T> constexpr spectate_ptr (spectate_ptr<T> that);
  constexpr spectate_ptr (std::nullptr_t) noexcept : spectate_ptr { } { };
  explicit spectate_ptr (pointer ptr) noexcept : ptr { ptr } { }

  spectate_ptr& operator = (std::nullptr_t) noexcept {
    this->reset();
    return *this;
  }

  explicit operator pointer () const noexcept { return this->ptr; }
  explicit operator bool () const noexcept { return this->ptr; }

  decltype(auto) operator * () const noexcept { return *this->ptr; }
  auto operator -> () const noexcept { return this->ptr; }

  void swap (spectate_ptr& that) noexcept {
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

//template <class T, class D>
//void swap (unique_ptr<T, D>& lhs, unique_ptr<T, D>& rhs) noexcept {
//  lhs.swap(rhs);
//}

template <class T, class R>
void swap (retain_ptr<T, R>& lhs, retain_ptr<T, R>& rhs) noexcept {
  lhs.swap(rhs);
}

template <class T>
void swap (spectate_ptr<T>& lhs, spectate_ptr<T>& rhs) noexcept { lhs.swap(rhs); }

template <class T, class U>
bool operator == (spectate_ptr<T> const& lhs, spectate_ptr<U> const& rhs) noexcept {
  return lhs.get() == rhs.get();
}

template <class T, class U>
bool operator != (spectate_ptr<T> const& lhs, spectate_ptr<U> const& rhs) noexcept {
  return lhs.get() != rhs.get();
}

template <class T, class U>
bool operator >= (spectate_ptr<T> const& lhs, spectate_ptr<U> const& rhs) noexcept {
  return lhs.get() >= rhs.get();
}

template <class T, class U>
bool operator <= (spectate_ptr<T> const& lhs, spectate_ptr<U> const& rhs) noexcept {
  return lhs.get() <= rhs.get();
}

template <class T, class U>
bool operator > (spectate_ptr<T> const& lhs, spectate_ptr<U> const& rhs) noexcept {
  return lhs.get() > rhs.get();
}

template <class T, class U>
bool operator < (spectate_ptr<T> const& lhs, spectate_ptr<U> const& rhs) noexcept {
  return lhs.get() < rhs.get();
}

template <class T>
bool operator == (spectate_ptr<T> const& lhs, ::std::nullptr_t) noexcept {
  return lhs.get() == nullptr;
}

template <class T>
bool operator != (spectate_ptr<T> const& lhs, ::std::nullptr_t) noexcept {
  return lhs.get() != nullptr;
}

template <class T>
bool operator == (::std::nullptr_t, spectate_ptr<T> const& rhs) noexcept {
  return nullptr == rhs.get();
}

template <class T>
bool operator != (::std::nullptr_t, spectate_ptr<T> const& rhs) noexcept {
  return nullptr != rhs.get();
}

/* shim definitions */
#if APEX_CHECK_API(to_address, 201711)
using ::std::to_address;
#else
template <class T>
constexpr auto to_address (T const& p) noexcept { return p.operator->(); }

template <class T>
constexpr T* to_address (T* p) noexcept { 
  static_assert(not std::is_function_v<T>);
  return p;
}
#endif /* APEX_CHECK_API(to_address, 201711) */

// This can't be constexpr under C++17 for a variety of reasons. However we
// don't really use it in such a manner, so it's fine for what we're doing
// here.
#if APEX_CHECK_API(constexpr_dynamic_alloc, 201907)
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
#endif /* APEX_CHECK_API(constexpr_dynamic_alloc, 201907) */

}} /* namespace apex::v1 */

#endif /* APEX_MEMORY_HPP */
