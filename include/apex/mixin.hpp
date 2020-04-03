#ifndef APEX_MIXIN_HPP
#define APEX_MIXIN_HPP

/* This header contains various mixins for iterators, creating a singleton */
namespace apex {
inline namespace v1 {

// TODO: Add comparison operator mixins

// TODO: add more noexcept capabilities
//       add static_assert checks for required mixed-in API
template <class> struct random_access_iterator;
template <class> struct bidirectional_iterator;
template <class> struct forward_iterator;
template <class> struct output_iterator;
template <class> struct input_iterator;

template <class T>
struct random_access_iterator : bidirectional_iterator<T> {
  using iterator_category = random_access_iterator_tag;
  using difference_type = decltype(declval<T>().distance_to(declval<T>()));

  decltype(auto) operator [] (difference_type n) const { return *(this->self() + n); } }
  difference_type operator - (random_access_iterator const& that) const {
    return self().distance_to(that.self());
  }

  derived operator - (difference_type n) const { return derived { self() } -= n; }
  derived operator + (difference_type n) const { return derived { self() } += n; }

  bool operator >= (random_access_iterator const& that) const { return not (*this < that); }
  bool operator <= (random_access_iterator const& that) const { return not (*this > that); }
  bool operator > (random_access_iterator const& that) const { return that < *this; }
  bool operator < (random_access_iterator const& that) const { return std::distance(*this, that) > 0; }
};

template <class T>
struct bidirectional_iterator : forward_iterator<T> {
  using iterator_category = bidirectional_iterator_tag;

  derived operator -- (int) {
    derived temp { self() };
    --*this;
    return temp;
  }

  derived& operator -- () {
    self().decrement();
    return self();
  }
};

template <class T>
struct forward_iterator : input_iterator<T> {
  using iterator_category = forward_iterator_tag;

  derived operator ++ () {
    derived temp { self() };
    ++*this;
    return temp;
  }
};

template <class T>
struct input_iterator {
  using iterator_category = input_iterator_tag;
  using difference_type = ptrdiff_t;
  using value_type = remove_cvref_t<declval<T>().dereference()>;
  using reference = decltype(declval<T>().dereference());
  using pointer = add_pointer_t<reference>; // TODO: Look this up for rangesv3?

  using derived = T;

  reference operator * () const { return self().dereference(); }
  pointer operator -> () const { return std::addressof(**this); }

  derived& operator ++ () {
    self().increment();
    return self();
  }

  bool operator == (input_iterator const& that) const {
    return self().equal(that.self());
  }

  bool operator != (input_iterator const& that) const {
    return not (*this == that);
  }
protected:
  derived const& self () const { return static_cast<derived*>(this); }
  derived& self () { return static_cast<derived*>(this); }
}

template <class T>
struct singleton {
  static_assert(std::is_default_constructible_v<T>);
  singleton (singleton const&) = delete;
  singleton () = delete;
  ~singleton () = delete;

  static T& global () noexcept {
    static T instance;
    return instance;
  }

  static T& local () noexcept {
    static thread_local T instance;
    return instance;
  }
};

template <class T, class D>
decltype(auto) operator + (D n, random_access_iterator<T> const& it) {
  return it + n;
}

template <class T, class D>
decltype(auto) operator - (D n, random_access_iterator<T> const& it) {
  return it - n;
}

template <class T> inline auto& global = singleton<T>::global();
template <class T> inline auto& local = singleton<T>::local();

template <class T> inline auto const& global<T const> = singleton<T>::global();
template <class T> inline auto const& local<T const> = singleton<T>::local();

}} /* namespace apex::v1 */

#endif /* APEX_MIXIN_HPP */
