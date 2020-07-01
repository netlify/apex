#ifndef APEX_CORE_SPAN_HPP
#define APEX_CORE_SPAN_HPP

#include <apex/core/concepts.hpp>
#include <apex/core/memory.hpp>
#include <apex/core/array.hpp>
#include <apex/core/prelude.hpp>

#include <apex/detect/iter.hpp>

#include <iterator>
#include <limits>

#include <cassert>

#if __has_include(<span>)
  #include <span>
#endif /* __has_include(<span>) */

namespace apex {

#if APEX_CHECK_API(span, 202002)
  using ::std::as_writable_bytes;
  using ::std::as_bytes;
  using ::std::dynamic_extent;
  using ::std::span;
#else

inline constexpr size_t dynamic_extent = std::numeric_limits<size_t>::max();

namespace impl {

// This is temporary *for now*
template <class R>
concept span_range = requires (R r) {
  { r.size() } -> convertible_to<size_t>;
};

template <class T, size_t N>
struct span final {
  constexpr span () noexcept = delete;

  constexpr span (T* ptr, size_t) noexcept :
    ptr { ptr }
  { }

  constexpr auto data () const noexcept { return this->ptr; }
  constexpr auto size () const noexcept { return N; }

private:
  T* ptr { };
};

template <class T>
struct span<T, dynamic_extent> final {
  constexpr span () noexcept = default;

  constexpr span (T* ptr, size_t len) noexcept :
    ptr { ptr },
    len { len }
  { }

  constexpr auto data () const noexcept { return this->ptr; }
  constexpr auto size () const noexcept { return this->len; }

private:
  T* ptr { };
  size_t len { };
};

} /* namespace impl */

template <complete_type T, size_t Extent=dynamic_extent>
struct span {
  static_assert(not std::is_abstract_v<T>);
  static_assert(std::is_object_v<T>);

  using difference_type = ptrdiff_t;
  using size_type = size_t;

  using element_type = T;
  using value_type = std::remove_cv_t<T>;

  using const_reference = std::add_lvalue_reference_t<std::add_const_t<T>>;
  using reference = std::add_lvalue_reference_t<T>;

  using const_pointer = std::add_pointer_t<std::add_const_t<T>>;
  using pointer = std::add_pointer_t<T>;

  using iterator = pointer;
  using reverse_iterator = std::reverse_iterator<iterator>;

  static constexpr size_t extent = Extent;

  /* not part of the official API. Still 'public' because why not */
  using storage_type = impl::span<T, extent>;
  template <size_t N>
  static constexpr bool safe_extent = extent == dynamic_extent or extent == N;

  template <size_t Offset, size_t Count>
  static constexpr auto subspan_extent = Count != dynamic_extent
    ? Count
    : (extent != dynamic_extent ? extent - Offset : dynamic_extent);
  template <size_t Offset, size_t Count>
  using subspan_t = span<element_type, subspan_extent<Offset, Count>>;

  constexpr span (nullptr_t, size_type) = delete;

  template <class It>
  constexpr span (It first, size_type count) noexcept :
    storage { apex::to_address(first), count }
  { }

  template <class It, class End> requires (not is_convertible_v<End, size_t>)
  constexpr span (It first, End last) noexcept :
    span { first, last - first }
  { }

  template <size_t N> requires safe_extent<N>
  constexpr span (element_type (&array)[N]) noexcept :
    span { array, N }
  { }

  template <class U, size_t N> requires safe_extent<N>
  constexpr span (std::array<U, N> const& array) noexcept :
    span { array.data(), N }
  { }

  template <class U, size_t N> requires safe_extent<N>
  constexpr span (std::array<U, N>& array) noexcept :
    span { array.data(), N }
  { }

  template <class R>
  requires requires (R r) {
    requires not std::is_array_v<remove_cvref_t<R>>
      or not is_bounded_specialization_of_v<remove_cvref_t<R>, std::array>
      or not is_bounded_specialization_of_v<remove_cvref_t<R>, span>;
    { r.size() } -> convertible_to<size_type>;
    { r.data() } -> convertible_to<pointer>;
  } constexpr span (R&& r) noexcept :
    span { std::data(std::forward<R>(r)), std::size(std::forward<R>(r)) }
  { }

  template <class U, size_t N>
  requires safe_extent<N> and (not same_as<U, T>)
  constexpr span (span<U, N> const& s) noexcept :
    span { s.data(), s.size() }
  { }

  constexpr span (span const&) noexcept = default;
  constexpr span () noexcept = default;

  constexpr span& operator = (span const&) noexcept = default;

  constexpr reference operator [] (size_type idx) const noexcept {
    return this->data()[idx];
  }

  constexpr reference front () const noexcept { return *this->begin(); }
  constexpr reference back () const noexcept { return *this->rbegin(); }

  constexpr iterator begin () const noexcept { return this->data(); }
  constexpr iterator end () const noexcept { return this->data() + this->size(); }

  constexpr reverse_iterator rbegin () const noexcept {
    return reverse_iterator { this->end() - 1 };
  }

  constexpr reverse_iterator rend () const noexcept {
    return reverse_iterator { this->begin() - 1 };
  }

  constexpr size_type size_bytes () const noexcept {
    return this->size() * sizeof(T);
  }

  constexpr size_type size () const noexcept { return this->storage.size(); }
  constexpr pointer data () const noexcept { return this->storage.data(); }

  [[nodiscard]] constexpr bool empty() const noexcept {
    return not this->size();
  }

  template <size_t Offset, size_t Count=dynamic_extent>
  constexpr subspan_t<Offset, Count> subspan () const noexcept {
    if constexpr (Count == dynamic_extent) {
      // TODO: this isn't currently handled...
    } else {
      return { this->data() + Offset, Count };
    }
  }

  template <size_t N>
  constexpr span<element_type, N> first () const noexcept {
    static_assert(extent >= N);
    if constexpr (extent == dynamic_extent) { assert(N <= this->size()); }
    return { this->data(), N };
  }

  template <size_t N>
  constexpr span<element_type, N> last () const noexcept {
    static_assert(extent >= N);
    if constexpr (extent == dynamic_extent) { assert(N <= this->size()); }
    return { this->rbegin() + N, N };
  }

  constexpr span<element_type> subspan (
    size_t offset,
    size_t count = dynamic_extent
  ) const noexcept;
  constexpr span<element_type> first (size_t count) const noexcept;
  constexpr span<element_type> last (size_t count) const noexcept;

private:
  storage_type storage;
};

template <class T, size_t N> span (std::array<T, N> const&) -> span<T, N>;
template <class T, size_t N> span (std::array<T, N>&) -> span<T, N>;
template <class T, size_t N> span (T(&)[N]) -> span<T, N>;

template <class It>
span (It, size_t) -> span<typename std::iterator_traits<It>::value_type>;
template <class It, class End>
span (It, End) -> span<typename std::iterator_traits<It>::value_type>;
template <class R>
span (R) -> span<typename R::value_type>;

extern template struct span<byte const, dynamic_extent>;
extern template struct span<byte, dynamic_extent>;

template <class T, size_t N>
requires (not is_const_v<T>)
span<byte, sizeof(T) * N> as_writable_bytes (span<T, N> s) noexcept {
  return { reinterpret_cast<std::byte*>(s.data()), s.size_bytes() };
}

template <class T> requires (not is_const_v<T>)
span<byte> as_writable_bytes (span<T> s) noexcept {
  return { reinterpret_cast<std::byte*>(s.data()), s.size_bytes() };
}

template <class T, size_t N>
span<byte const, sizeof(T) * N> as_bytes (span<T, N> s) noexcept {
  return { reinterpret_cast<std::byte const*>(s.data()), s.size_bytes() };
}

template <class T>
span<byte const> as_bytes (span<T> s) noexcept {
  return { reinterpret_cast<std::byte const*>(s.data()), s.size_bytes() };
}

template <class T, size_t N>
void swap (span<T, N>& lhs, span<T, N>& rhs) noexcept { lhs.swap(rhs); }

#endif /* APEX_CHECK_API(span) */

} /* namespace apex */

#endif /* APEX_CORE_SPAN_HPP */
