#ifndef APEX_MIXIN_HPP
#define APEX_MIXIN_HPP

/* This header contains various mixins for iterators, creating a singleton,
 * etc.
 * When using types from this header it is *expected* that you will inherit
 * from them. They should *never* appear in any function parameters, but
 * can appear within functions to make things easier.
 */
// TODO: Place everything here within apex::mixin namespace
namespace apex {
inline namespace v1 {

// Compresses all empty bases in Ts..., such that they are all placed at the
// end of the inheritance 'tree'. Effectively a guaranteed EBO capable
// std::tuple but with a massively reduced interface for our sanity.
// TODO: Implement
template <class... Ts>
struct compressed {
  constexpr compressed (compressed const&) noexcept = default;
  constexpr compressed (compressed&&) noexcept = default;

  // Anything that can have get<N> and tuple_size called on it
  template <class U> constexpr compressed (U&& applicable) noexcept;
};

template <class... Ts> compressed (Ts...) -> compressed<Ts...>;

// Intended to be used like:
/*
 * template <class T, class D=std::default_delete<T>>
 * using unique_handle = mixin::handle<T, std::unique_ptr<T, D>>;
 * struct my_type : unique_handle<int> { }
 */
// This saves a LOT of time and energy when wrapping C APIs, of which there are
// a great deal of in the world.
template <class T, class Storage>
struct handle {
  using storage_type = Storage;
  // Attempts to set 'pointer' to the following in order
  // typename storage_type::pointer,
  // add_pointer_t<typename storage_type::element_type>
  // add_pointer_t<T>
  using pointer = detected_or_t<
    add_pointer_t<detected_or_t<T, detect::element_type, storage_type>>,
    detect::pointer,
    storage_type
  >;

  static_assert(std::is_default_constructible_v<storage_type>);
  static_assert(std::is_move_constructible_v<storage_type>);
  static_assert(std::is_convertible_v<bool, storage_type>);
  static_assert(std::is_swappable_v<storage_type>);
  static_assert(is_detected_v<detect::op::dereference, storage_type>);
  static_assert(is_detected_v<detect::op::arrow, storage_type>);

  handle (std::nullptr_t) noexcept : handle { } { }
  handle () noexcept = default;

  explicit operator bool () const noexcept { return bool(this->storage); }
  decltype(auto) get () const noexcept { return this->storage.get(); }

  template <class... Args>
  void reset (pointer ptr, Args&&... args) noexcept {
    this->storage.reset(ptr, std::forward<Args>(args)...);
  }
  void reset () noexcept { this->storage.reset(); }

  void swap (handle& that) noexcept {
    using std::swap;
    swap(this->storage, that.storage);
  }

protected:
  template <
    class... Args,
    class=std::enable_if_t<std::is_constructible_v<storage_type, pointer, Args...>>
  > handle (pointer ptr, Args&&...) :
    storage { ptr, std::forward<Args>(args)... }
  { }

  storage_type storage;
};

// TODO: Add comparison operator mixin

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
  using value_type = remove_cvref_t<decltype(declval<T>().dereference())>;
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

/* Specializations for structured bindings and other things */
namespace std {

// TODO: tuple_element specialization
template <class Ts...>
class tuple_size<apex::compressed<Ts...>> :
  public integral_constant<size_t, sizeof...(Ts)>
{ };

} /* namespace std */

#endif /* APEX_MIXIN_HPP */
