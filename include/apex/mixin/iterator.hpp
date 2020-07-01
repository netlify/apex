#ifndef APEX_MIXIN_ITERATOR_HPP
#define APEX_MIXIN_ITERATOR_HPP


#include <apex/core/iterator.hpp>
#include <apex/core/concepts.hpp>
#include <apex/proxy/arrow.hpp>

// TODO: Write an *even better* version of this
// See https://vector-of-bool.github.io/2020/06/13/cpp20-iter-facade.html 
// for more details

//namespace apex::mixin::detail {
//
//template <class I>
//concept distance_to = requires (remove_cvref_t<I> const& i) {
//  { i.distance_to(i) } -> different_from<void>;
//};
//
//template <class I>
//concept equal_to = requires(remove_cvref_t<I> const& i) {
//  { i.equal_to(i) } -> boolean;
//};
//
//template <class I>
//concept dereference = requires (remove_cvref_t<I>& self) { self.dereference(); };
//
//template <class I>
//concept next = requires (remove_cvref_t<I>& self) {
//  { self.next() } -> same_as<void>;
//};
//
//template <class I>
//concept prev = requires (remove_cvref_t<I>& self) {
//  { self.prev() } -> same_as<void>;
//};
//
//template <class I>
//concept advance = false;requires (remove_cvref_t<I>& i, remove_cvref_t<D> d) {
//  { i.advance(d) } -> same_as<void>;
//};
//
//template <class I>
//concept input_iterator = requires {
//  requires dereference<I>;
//  requires equal_to<I>;
//  requires next<I>;
//};
//
//template <class I>
//concept forward_iterator = requires {
//  requires copy_constructible<I>;
//  requires input_iterator<I>;
//};
//
//template <class I>
//concept bidirectional_iterator = requires {
//  requires forward_iterator<I>;
//  requires prev<I>;
//};
//
//template <class I>
//concept random_access_iterator = requires {
//  requires dereference<I>;
//  requires distance_to<I>;
//  requires apex::mixin::detail::advance<I>;
//};
//
//template <class> struct category { };
//
//template <input_iterator I>
//struct category<I> :
//  type_identity<std::input_iterator_tag>
//{ };
//
//template <forward_iterator I>
//struct category<I> :
//  type_identity<std::forward_iterator_tag>
//{ };
//
//template <bidirectional_iterator I>
//struct category<I> :
//  type_identity<std::bidirectional_iterator_tag>
//{ };
//
//template <random_access_iterator I>
//struct category<I> :
//  type_identity<std::random_access_iterator_tag>
//{ };
//
//template <class T> using category_t = typename category<T>::type;
//
//} /* namespace apex::mixin::detail */
//
//namespace apex::mixin {
//
//template <class Derived>
//struct iterator {
//  using iterator_type = iterator;
//  using self_type = Derived;
//
//  decltype(auto) operator * () const noexcept { return this->self().dereference(); }
//
//  auto operator -> () const
//  noexcept(noexcept(std::addressof(**this)))
//  requires is_lvalue_reference_v<decltype(**declval<iterator_type const&>())> {
//    return std::addressof(**this);
//  }
//
////  auto operator -> () const&
////  noexcept(is_nothrow_copy_constructible_v<self_type>)
////  requires detail::reference_to_value<I> {
////    return proxy::arrow(**this);
////  }
////
////  auto operator -> () &&
////  noexcept(is_nothrow_move_constructible_v<self_type>)
////  requires detail::reference_to_value<I> {
////    return proxy::arrow(**this);
////  }
//
//  self_type& operator ++ ()
//  noexcept(noexcept(this->self().next()))
//  requires detail::next<self_type> {
//    this->self().next();
//    return this->self();
//  }
//
//  self_type& operator -- ()
//  noexcept(noexcept(this->self().prev()))
//  requires detail::prev<self_type> {
//    this->self().prev();
//    return this->self();
//  }
//
//  self_type operator ++ (int)
//  noexcept(is_nothrow_copy_constructible_v<self_type>)
//  requires copy_constructible<self_type> {
//    auto that = this->self();
//    ++*this;
//    return that;
//  }
//
//  self_type operator -- (int)
//  noexcept(is_nothrow_copy_constructible_v<self_type>)
//  requires copy_constructible<self_type> {
//    auto that = this->self();
//    --*this;
//    return that;
//  }
//
//  friend bool operator == (self_type const& lhs, self_type const& rhs)
//  noexcept(noexcept(lhs.equal_to(rhs)))
//  requires apex::mixin::detail::equal_to<self_type>
//  { return lhs.equal_to(rhs); }
//
//private:
//  decltype(auto) self () const noexcept { return *static_cast<self_type const*>(*this); }
//  decltype(auto) self () noexcept { return *static_cast<self_type*>(*this); }
//};
//
//} /* namespace apex::mixin */
//
//template <class Iter> requires apex::derived_from<Iter, apex::mixin::iterator<Iter>>
//struct std::iterator_traits<Iter> {
//  using difference_type = apex::mixin::detail::difference_type<Iter>;
//  using value_type = apex::mixin::detail::value_type<Iter>;
//  using reference = decltype(std::declval<Iter>());
//  using pointer = decltype(std::declval<Iter>().operator ->());
//
//  using iterator_category = apex::mixin::detail::category_t<Iter>;
//  using iterator_concept = iterator_category;
//};

// XXX: This mixin is kind of a mess because we don't have all the ranges::
// concepts available to us. Currently we don't guarantee that
// iterator_traits::dereference returns a reference which turns operator ->
// into a "bad" call otherwise. As a result, we need to support proxy objects
// at some point. ranges supports this. We, however, do not :(
namespace apex::mixin {

using std::declval;

template <class T> using sentinel_type = remove_cvref_t<typename T::sentinel_type>;

// TODO: 
template <class I>
concept iterator_dereference = requires (cref_t<I> i) { i.dereference(); };
template <class I>
concept iterator_increment = requires (remove_cvref_t<I>& i) {
  { i.increment() } -> same_as<void>;
};
template <class I, class S>
concept iterator_equals = requires (cref_t<I> i, cref_t<S> s) {
  { i.equals(s) } -> boolean;
};
template <class I>
concept iterator_decrement = requires (remove_cvref_t<I>& i) {
  { i.decrement() } -> same_as<void>;
};
template <class I, class D>
concept iterator_advance = requires (remove_cvref_t<I>& i, remove_cvref_t<D> d) {
  { i.advance(d) } -> same_as<void>;
};
template <class I, class S>
concept iterator_distance_to = requires (cref_t<I> i, cref_t<S> s) {
  { i.distance_to(s) } -> same_as<iter_difference_t<remove_cvref_t<I>>>;
};

// TODO: rewrite iterator_traits in terms of the iterator mixin, rather than
// the other way around!

// Used as a default traits type to 'get' implementation information for the
// iterator mixin. This is needed due to how CRTP is implemented within the
// language.
// While this *is* used as the default, users are free to substitute their own
// traits type.
// TODO: we need more iter_XXX_t traits/concepts from C++20 to make this rock solid :(
// minimum interface needed is dereference, increment, equals
template <class T>
struct iterator_traits {
  using iterator_type = remove_cvref_t<T>;
  using sentinel_type = detected_or_t<iterator_type, sentinel_type, iterator_type>;

  // This *should* just be iter_difference_t
  using difference_type = iter_difference_t<iterator_type>;

  [[nodiscard]] static decltype(auto) dereference (iterator_type const& iter)
    noexcept(noexcept(iter.dereference()))
    requires iterator_dereference<iterator_type>
  { return iter.dereference(); }

  static void increment (iterator_type& iter)
    noexcept(noexcept(iter.increment()))
    requires iterator_increment<iterator_type>
  { iter.increment(); }

  static void decrement (iterator_type& iter)
    noexcept(noexcept(iter.decrement()))
    requires iterator_decrement<iterator_type>
  { iter.decrement(); }

  static void advance (iterator_type& iter, difference_type n)
    requires iterator_advance<iterator_type, difference_type>
  { iter.advance(n); }

  [[nodiscard]] static decltype(auto) distance_to (iterator_type const& begin, sentinel_type const& end)
    requires iterator_distance_to<iterator_type, sentinel_type>
  { return begin.distance_to(end); }

  [[nodiscard]] static decltype(auto) equals (iterator_type const& begin, sentinel_type const& end)
    requires iterator_equals<iterator_type, sentinel_type>
  { return begin.equals(end); }

};

template <class I>
concept input_iterator = requires {
  typename iterator_traits<I>::iterator_type;
  requires iterator_dereference<I>;
  requires iterator_increment<I>;
  requires iterator_equals<
    typename iterator_traits<I>::iterator_type,
    typename iterator_traits<I>::sentinel_type
  >;
};

template <class I>
concept forward_iterator = requires {
  requires copy_constructible<I>;
  requires input_iterator<I>;
};

template <class I>
concept bidirectional_iterator = requires {
  requires forward_iterator<I>;
  requires iterator_decrement<I>;
};

template <class I>
concept random_access_iterator = requires {
  typename iterator_traits<I>::difference_type;
  requires not same_as<typename iterator_traits<I>::difference_type, empty>;
  requires bidirectional_iterator<I>;
  requires iterator_advance<I, typename iterator_traits<I>::difference_type>;
  requires iterator_distance_to<
    typename iterator_traits<I>::iterator_type,
    typename iterator_traits<I>::sentinel_type
  >;
};

template <class T, class I=iterator_traits<T>>
struct iterator {
  using derived_type = T;
  using traits_type = I;

  using difference_type = typename traits_type::difference_type;
  using sentinel_type = typename traits_type::sentinel_type;

  // input-iterator
  decltype(auto) operator * () const noexcept(
    noexcept(traits_type::dereference(this->self()))
  ) { return traits_type::dereference(this->self()); }

  // input-iterator
  decltype(auto) operator -> () const noexcept(
    noexcept(traits_type::addressof(this->self()))
  ) { return traits_type::addressof(this->self()); }

  // random-access-iterator
  decltype(auto) operator [] (difference_type n) const noexcept(
    noexcept(*this->self() + n)
  ) { return *(this->self() + n); }

  // random-access-iterator
  decltype(auto) operator += (difference_type n)
    noexcept(noexcept(traits_type::advance(this->self(), n)))
  { traits_type::advance(this->self(), -n); return this->self(); }

  // random-access-iterator
  decltype(auto) operator -= (difference_type n)
    noexcept(noexcept(traits_type::advance(this->self(), -n)))
    requires random_access_iterator<derived_type>
  { traits_type::advance(this->self(), -n); return this->self(); }

  // random-access-iterator
  decltype(auto) operator - (sentinel_type const& that) const
    noexcept(noexcept(traits_type::distance_to(this->self(), that)))
    requires random_access_iterator<derived_type>
  { return traits_type::distance_to(this->self(), that); }

  // random-access iterator
  // TODO: correct noexcept to also check for -= noexcept
  derived_type operator - (difference_type n) const noexcept(
    std::is_nothrow_copy_constructible_v<derived_type>
  ) { return derived_type { this->self() } -= n; }

  // random-access-iterator
  // TODO: correct noexcept to also check for += noexcept
  derived_type operator + (difference_type n) const noexcept(
    std::is_nothrow_copy_constructible_v<derived_type>
  ) { return derived_type { this->self() } += n; }

  // bidirectional-iterator
  derived_type operator -- (int) noexcept(std::is_nothrow_copy_constructible_v<derived_type>) {
    derived_type that { this->self() };
    --*this;
    return that;
  }

  // bidirectional-iterator
  derived_type& operator -- () noexcept(noexcept(traits_type::decrement(this->self()))) {
    traits_type::decrement(this->self());
    return this->self();
  }

  // forward-iterator
  derived_type operator ++ (int) noexcept(std::is_nothrow_copy_constructible_v<derived_type>) {
    derived_type that { this->self() };
    ++*this;
    return that;
  }

  // input-iterator
  derived_type& operator ++ () noexcept(
    noexcept(traits_type::increment(this->self()))
  ) {
    traits_type::increment(this->self());
    return this->self();
  }

  // input-iterator
  bool operator == (iterator const& that) const noexcept(
    noexcept(traits_type::equals(this->self(), that))
  ) { return traits_type::equals(this->self(), that); }

  // input-iterator
  bool operator != (iterator const& that) const noexcept(
    noexcept(not (this->self() == that))
  ) { return not (this->self() == that); }

  // TODO: update these to be noexcept-safe and compare against sentinel_type
  // random-access-iterator
  bool operator >= (iterator const& that) const noexcept {
    return not (*this < that);
  }
  // random-access-iterator
  bool operator <= (iterator const& that) const noexcept {
    return not (*this > that);
  }
  // random-access-iterator
  bool operator > (iterator const& that) const noexcept {
    return that < *this;
  }
  // random-access-iterator
  bool operator < (iterator const& that) const noexcept {
    return std::distance(*this, that) > 0;
  }

private:
  decltype(auto) self () const noexcept { return *static_cast<T const*>(this); }
  decltype(auto) self () noexcept { return *static_cast<T*>(this); }
};

template <class T, class D>
decltype(auto) operator + (D n, iterator<T> const& it) noexcept(noexcept(it + n)) {
  return it + n;
}

template <
  class T,
  class D,
  class=std::enable_if_t<
    std::is_same_v<
      typename std::iterator_traits<iterator<T>>::iterator_tag,
      std::random_access_iterator_tag
    >
  >
> decltype(auto) operator - (D n, iterator<T> const& it) noexcept(noexcept(it - n)) {
  return it - n;
}

template <class> struct output_iterator { };

//template <class T, class D>
//decltype(auto) operator + (D n, random_access_iterator<T> const& it) noexcept(noexcept(it + n)) {
//  return it + n;
//}
//
//template <class T, class D>
//decltype(auto) operator - (D n, random_access_iterator<T> const& it) noexcept(noexcept(it - n)) {
//  return it - n;
//}

} /* namespace apex::mixin */

#endif /* APEX_MIXIN_ITERATOR_HPP */
