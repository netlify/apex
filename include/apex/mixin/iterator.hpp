#ifndef APEX_MIXIN_ITERATOR_HPP
#define APEX_MIXIN_ITERATOR_HPP

#include <apex/core/iterator.hpp>
#include <apex/core/concepts.hpp>
#include <apex/proxy/arrow.hpp>

#include <apex/iter/concepts.hpp>

// TODO: Write an *even better* version of this
// See https://vector-of-bool.github.io/2020/06/13/cpp20-iter-facade.html 
// for more details
namespace apex::mixin::temp {

// FIXME: We need to make sure the T here is at a minimum an indirectly_readable
// or at the very least in our case, indirectly_readable_from (which is just our version of it)
template <class T>
struct iterator {
  using derived_type = T;
  using sentinel_type = iter::sentinel_of_t<derived_type>;
  // TODO: detect the difference type by calling distance_to, or just
  // don't create a difference_type
  using difference_type = ptrdiff_t;
  //using difference_type = iter_difference_t<derived_type>;

  decltype(auto) operator -> () const noexcept(noexcept(**this))
  requires iter::indirectly_addressable<derived_type> {
    return **this;
  }

  decltype(auto) operator -> () const noexcept(noexcept(**this))
  requires iter::indirectly_readable_from<derived_type> {
    return proxy::arrow(std::move(**this));
  }

  decltype(auto) operator * () const
  noexcept(noexcept(::apex::iter::read_from(this->self())))
  requires iter::indirectly_readable_from<derived_type> {
    return ::apex::iter::read_from(this->self());
  }

  decltype(auto) operator [] (difference_type n) const
  noexcept(noexcept(::apex::iter::advance(this->self(), n)))
  requires iter::weakly_steppable<derived_type, difference_type> {
    return *(this->self() + n);
  }

  decltype(auto) operator += (difference_type n)
  noexcept(noexcept(::apex::iter::advance(this->self(), n))) 
  requires iter::weakly_steppable<derived_type, difference_type> {
    ::apex::iter::advance(this->self(), n);
    return this->self();
  }

  decltype(auto) operator -= (difference_type n)
  noexcept(noexcept(::apex::iter::advance(this->self(), n)))
  requires iter::weakly_steppable<derived_type, difference_type> {
    ::apex::iter::advance(this->self(), -n);
    return this->self();
  }

  decltype(auto) operator + (difference_type n) const
  noexcept(::std::is_nothrow_copy_constructible_v<derived_type> and noexcept(this->self() += n))
  requires iter::weakly_steppable<derived_type, difference_type> {
    return derived_type { this->self() } += n;
  }

  decltype(auto) operator - (difference_type n) const
  noexcept(::std::is_nothrow_copy_constructible_v<derived_type> and noexcept(this->self() -= n))
  requires iter::weakly_steppable<derived_type, difference_type> {
    return derived_type { this->self() } -= n;
  }

  decltype(auto) operator ++ (int)
  noexcept(::std::is_nothrow_copy_constructible_v<derived_type> and noexcept(++*this))
  requires iter::weakly_decrementable<derived_type> and copy_constructible<derived_type> {
    auto that = this->self();
    ++*this;
    return that;
  }

  decltype(auto) operator ++ ()
  noexcept(noexcept(::apex::iter::next(this->self())))
  requires iter::weakly_incrementable<derived_type> {
    ::apex::iter::next(this->self());
    return this->self();
  }

  decltype(auto) operator -- (int)
  noexcept(::std::is_nothrow_copy_constructible_v<derived_type> and noexcept(--*this))
  requires iter::weakly_decrementable<derived_type> and copy_constructible<derived_type> {
    auto that = this->self();
    --*this;
    return that;
  }

  decltype(auto) operator -- ()
  noexcept(noexcept(::apex::iter::prev(this->self())))
  requires iter::weakly_decrementable<derived_type> {
    ::apex::iter::prev(this->self());
    return this->self();
  }

  bool operator == (sentinel_type const& that) const
  noexcept(noexcept(::apex::iter::equal_to(this->self(), that)))
  requires iter::equality_comparable<derived_type, sentinel_type> {
    return ::apex::iter::equal_to(this->self(), that);
  }

  bool operator != (sentinel_type const& that) const noexcept(noexcept(not (*this == that)))
  requires iter::equality_comparable<derived_type, sentinel_type> {
    return not (*this == that);
  }

  bool operator >= (sentinel_type const& that) const noexcept(noexcept(not (*this < that)))
  requires iter::distance_measurable<derived_type, sentinel_type> {
    return not (*this < that);
  }

  bool operator <= (sentinel_type const& that) const noexcept(noexcept(not (*this > that)))
  requires iter::distance_measurable<derived_type, sentinel_type> {
    return not (*this > that);
  }

  bool operator > (sentinel_type const& that) const noexcept(noexcept(that < *this))
  requires iter::distance_measurable<derived_type, sentinel_type> {
    return that < *this;
  }

  bool operator < (sentinel_type const& that) const
  noexcept(noexcept(::apex::iter::distance_to(*this, that)))
  requires iter::distance_measurable<derived_type, sentinel_type> {
    return ::apex::iter::distance_to(*this, that) > 0;
  }

private:
  decltype(auto) self () const noexcept { return *static_cast<T const*>(this); }
  decltype(auto) self () noexcept { return *static_cast<T*>(this); }
};


template <class T>
decltype(auto) operator + (iter_difference_t<T> n, iterator<T> const& it) noexcept(noexcept(it + n)) {
  return it + n;
}

} /* namespace apex::mixin */

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

  decltype(auto) operator - (sentinel_type const& that) const
    noexcept(noexcept(traits_type::distance_to(this->self(), that)))
    requires random_access_iterator<derived_type>
  { return traits_type::distance_to(this->self(), that); }
};

template <class T, class D>
decltype(auto) operator + (D n, iterator<T> const& it) noexcept(noexcept(it + n)) {
  return it + n;
}

} /* namespace apex::mixin */

#endif /* APEX_MIXIN_ITERATOR_HPP */
