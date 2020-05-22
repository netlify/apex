#ifndef APEX_MIXIN_ITERATOR_HPP
#define APEX_MIXIN_ITERATOR_HPP

#include <apex/core/traits.hpp>
#include <iterator>

namespace apex {
inline namespace v1 {

using std::declval;

// TODO: make a single iterator mixin
// TODO: Use iterator_traits to implement most of it

template <class> struct random_access_iterator;
template <class> struct bidirectional_iterator;
template <class> struct forward_iterator;
template <class> struct output_iterator;
template <class> struct input_iterator;

// TODO: static_assert(is_detected_v<has_distance_to, derived>);
template <class T>
struct random_access_iterator : bidirectional_iterator<T> {
  using iterator_category = std::random_access_iterator_tag;
  using difference_type = decltype(declval<T>().distance_to(declval<T>()));
  using derived = typename bidirectional_iterator<T>::derived;

  static_assert(std::is_signed_v<difference_type>);

  static constexpr bool nothrow_copyable = std::is_nothrow_copy_constructible_v<derived>;

  decltype(auto) operator [] (difference_type n) const { return *(this->self() + n); }

  decltype(auto) operator += (difference_type n) const {
    this->self().advance(n);
    return this->self();
  }

  decltype(auto) operator -= (difference_type n) const {
    this->self().advance(-n);
    return this->self();
  }

  auto operator - (random_access_iterator const& that) const {
    return this->self().distance_to(that.self());
  }

  derived operator - (difference_type n) const noexcept(nothrow_copyable) {
    return derived { this->self() } -= n;
  }

  derived operator + (difference_type n) const noexcept(nothrow_copyable) {
    return derived { this->self() } += n;
  }

  bool operator >= (random_access_iterator const& that) const noexcept {
    return not (*this < that);
  }

  bool operator <= (random_access_iterator const& that) const noexcept {
    return not (*this > that);
  }

  bool operator > (random_access_iterator const& that) const noexcept {
    return that < *this;
  }

  bool operator < (random_access_iterator const& that) const noexcept {
    return std::distance(*this, that) > 0;
  }
};

// TODO: static_assert(is_detected_v<has_decrement>);
template <class T>
struct bidirectional_iterator : forward_iterator<T> {
  using iterator_category = std::bidirectional_iterator_tag;
  using derived = typename forward_iterator<T>::derived;

  derived operator -- (int) noexcept(std::is_nothrow_copy_constructible_v<derived>) {
    derived temp { this->self() };
    --*this;
    return temp;
  }

  derived& operator -- () noexcept(noexcept(std::declval<derived>().decrement())) {
    this->self().decrement();
    return this->self();
  }
};

template <class T>
struct forward_iterator : input_iterator<T> {
  using iterator_category = std::forward_iterator_tag;
  using derived = typename input_iterator<T>::derived;

  derived operator ++ () noexcept(std::is_nothrow_copy_constructible_v<derived>) {
    derived temp { this->self() };
    ++*this;
    return temp;
  }
};

template <class T>
using has_dereference = decltype(declval<T>().dereference());

template <class T>
struct input_iterator {
  using iterator_category = std::input_iterator_tag;
  using difference_type = ptrdiff_t;
  using value_type = remove_cvref_t<detected_t<has_dereference, T>>;
  using reference = detected_t<has_dereference, T>;
  using pointer = std::add_pointer_t<reference>; // TODO: look this up for rangesv3?

  using derived = T;

  reference operator * () const noexcept(noexcept(declval<derived>().dereference())) {
    return self().dereference();
  }
  pointer operator -> () const noexcept(noexcept(declval<derived>().dereference())) {
    return std::addressof(**this);
  }

  derived& operator ++ () noexcept(noexcept(declval<derived>().increment())) {
    self().increment();
    return self();
  }

  bool operator == (input_iterator const& that) const noexcept(
    noexcept(declval<derived>().equal(declval<derived>()))
  ) { return self().equal(that.self()); }
protected:
  derived const& self () const { return *static_cast<derived const*>(this); }
  derived& self () { return *static_cast<derived*>(this); }
};

template <class T, class D>
decltype(auto) operator + (D n, random_access_iterator<T> const& it) noexcept(noexcept(it + n)) {
  return it + n;
}

template <class T, class D>
decltype(auto) operator - (D n, random_access_iterator<T> const& it) noexcept(noexcept(it - n)) {
  return it - n;
}

}} /* namespace apex::v1 */

#endif /* APEX_MIXIN_ITERATOR_HPP */
