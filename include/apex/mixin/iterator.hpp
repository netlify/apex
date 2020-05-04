#ifndef APEX_MIXIN_ITERATOR_HPP
#define APEX_MIXIN_ITERATOR_HPP

namespace apex {
inline namespace v1 {

template <class> struct random_access_iterator;
template <class> struct bidirectional_iterator;
template <class> struct forward_iterator;
template <class> struct output_iterator;
template <class> struct input_iterator;

// TODO: static_assert(is_detected_v<has_distance_to, derived>);
template <class T>
struct random_access_iterator : bidirectional_iterator<T> {
  using iterator_category = random_access_iterator_tag;
  using difference_type = decltype(declval<T>().distance_to(declval<T>()));

  static_assert(std::is_signed_v<difference_type>);

  static constexpr bool nothrow_copyable = std::is_nothrow_copy_constructible_v<derived>;

  decltype(auto) operator [] (difference_type n) const { return *(self() + n); }

  auto operator - (random_access_iterator const& that) const {
    return self().distance_to(that.self());
  }

  derived operator - (difference_type n) const noexcept(nothrow_copyable) {
    return derived { self() } -= n;
  }

  derived operator + (difference_type n) const noexcept(nothrow_copyable) {
    return derived { self() } += n;
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

  bool operator < (random_acccess_iterator const& that) const noexcept {
    return std::distance(*this, that) > 0;
  }
};

// TODO: static_assert(is_detected_v<has_decrement>);
template <class T>
struct bidirectional_iterator : forward_iterator<T> {
  using iterator_category = bidirectional_iterator_tag;

  derived operator -- (int) noexcept(std::is_nothrow_copy_constructible_v<derived>) {
    derived temp { self() };
    --*this;
    return temp;
  }

  derived& operator -- () noexcept(noexcept(std::declval<derived>().decrement())) {
    self().decrement();
    return self;
  }
};

template <class T>
struct forward_iterator : input_iterator<T> {
  using iterator_category = forward_iterator_tag;

  derived operator ++ () noexcept(std::is_nothrow_copy_constructible_v<derived>) {
    derived temp { self() };
    ++*this;
    return temp;
  }
};

template <class T>
struct input_iterator {
  using iterator_category = input_iterator_tag;
  using difference_type = ptrdiff_t;
  using value_type = remove_cvref_t<decltype(declval<T>().dereference())>;
  using reference = decltype(declval<T>().dereference());
  using pointer = add_pointer_t<reference>; // TODO: look this up for rangesv3?

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
