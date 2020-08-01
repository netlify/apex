#ifndef APEX_MEMORY_RETAIN_HPP
#define APEX_MEMORY_RETAIN_HPP

#include <apex/core/concepts.hpp>
#include <apex/core/meta.hpp>

#include <memory>
#include <atomic>

namespace apex::detail {

template <class T> using reset_action = typename T::reset_action;
template <class T, class U>
concept use_count = requires (T ptr) {
  { U::use_count(ptr) } -> signed_integral;
};

} /* apex::detail */

// This is an implementation of P0468
namespace apex {

struct retain_t { constexpr retain_t () noexcept = default; };
struct adopt_t { constexpr adopt_t () noexcept = default; };

inline constexpr retain_t retain { };
inline constexpr adopt_t adopt { };

template <class T>
struct atomic_reference_count {
  template <class> friend struct retain_traits;
protected:
  atomic_reference_count () = default;
private:
  std::atomic<long> count { 1 };
};

template <class T>
struct reference_count {
  template <class> friend struct retain_traits;
protected:
  reference_count () = default;
private:
  long count { 1 };
};

template <class T>
struct retain_traits final {

  template <derived_from<T> U>
  static void increment (atomic_reference_count<U>* ptr) {
    ptr->count.fetch_add(1, std::memory_order_relaxed);
  }

  template <derived_from<T> U>
  static void increment (reference_count<U>* ptr) noexcept {
    ++ptr->count;
  }

  template <derived_from<T> U>
  static void decrement (atomic_reference_count<U>* ptr) noexcept {
    ptr->count.fetch_sub(1, std::memory_order_acq_rel);
    if (not use_count(ptr)) { delete static_cast<T*>(ptr); }
  }

  template <derived_from<T> U>
  static void decrement (reference_count<U>* ptr) noexcept {
    --ptr->count;
    if (not use_count(ptr)) { delete static_cast<T*>(ptr); }
  }

  template <derived_from<T> U>
  static long use_count (atomic_reference_count<U>* ptr) noexcept {
    return ptr->count.load(std::memory_order_relaxed);
  }

  template <derived_from<T> U>
  static long use_count (reference_count<U>* ptr) noexcept {
    return ptr->count;
  }
};

template <class T, class R=retain_traits<T>>
struct retain_ptr {
  using element_type = T;
  using traits_type = R;

  using pointer = detected_or_t<
    std::add_pointer_t<element_type>,
    meta::pointer,
    traits_type
  >;

  using reset_action = detected_or_t<adopt_t, detail::reset_action, traits_type>;

  static_assert(std::disjunction_v<
    std::is_same<reset_action, retain_t>,
    std::is_same<reset_action, adopt_t>
  >);

  retain_ptr (pointer ptr, retain_t) noexcept :
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
    retain_ptr { that.release() }
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

  [[clang::reinitializes]] retain_ptr& operator = (nullptr_t) noexcept {
    this->reset();
    return *this;
  }

  void swap (retain_ptr& that) noexcept {
    using std::swap;
    swap(this->ptr, that.ptr);
  }

  explicit operator bool () const noexcept
    requires convertible_to<pointer, bool>
  { return this->get(); }
  decltype(auto) operator * () const noexcept { return *this->get(); }
  pointer operator -> () const noexcept { return this->get(); }

  pointer get () const noexcept { return this->ptr; }

  [[nodiscard, clang::reinitializes]] pointer release () noexcept {
    return std::exchange(this->ptr, pointer { });
  }

  void reset (pointer ptr, retain_t) noexcept {
    *this = retain_ptr(ptr, retain);
  }

  void reset (pointer ptr, adopt_t) noexcept {
    *this = retain_ptr(ptr, adopt);
  }

  void reset (pointer ptr = pointer { }) noexcept {
    *this = retain_ptr(ptr, reset_action { });
  }

  auto use_count () const noexcept requires detail::use_count<pointer, traits_type> {
    return traits_type::use_count(this->get());
  }

private:
  pointer ptr;
};

template <class T, class R>
void swap (retain_ptr<T, R>&  lhs, retain_ptr<T, R>& rhs) noexcept {
  lhs.swap(rhs);
}

template <class T, class R>
bool operator == (retain_ptr<T, R> const& lhs, retain_ptr<T, R> const& rhs) noexcept {
  return lhs.get() == rhs.get();
}

template <class T, class R>
bool operator != (retain_ptr<T, R> const& lhs, retain_ptr<T, R> const& rhs) noexcept {
  return lhs.get() != rhs.get();
}

template <class T, class R>
bool operator >= (retain_ptr<T, R> const& lhs, retain_ptr<T, R> const& rhs) noexcept {
  return lhs.get() >= rhs.get();
}

template <class T, class R>
bool operator <= (retain_ptr<T, R> const& lhs, retain_ptr<T, R> const& rhs) noexcept {
  return lhs.get() <= rhs.get();
}

template <class T, class R>
bool operator > (retain_ptr<T, R> const& lhs, retain_ptr<T, R> const& rhs) noexcept {
  return lhs.get() > rhs.get();
}

template <class T, class R>
bool operator < (retain_ptr<T, R> const& lhs, retain_ptr<T, R> const& rhs) noexcept {
  return lhs.get() < rhs.get();
}

template <class T, class R>
bool operator == (retain_ptr<T, R> const& lhs, nullptr_t) noexcept {
  return lhs.get() == nullptr;
}

template <class T, class R>
bool operator != (retain_ptr<T, R> const& lhs, nullptr_t) noexcept {
  return lhs.get() != nullptr;
}

template <class T, class R>
bool operator == (nullptr_t, retain_ptr<T, R> const& rhs) noexcept {
  return nullptr == rhs.get();
}

template <class T, class R>
bool operator != (nullptr_t, retain_ptr<T, R> const& rhs) noexcept {
  return nullptr != rhs.get();
}

} /* namespace apex */

#endif /* APEX_MEMORY_RETAIN_HPP */
