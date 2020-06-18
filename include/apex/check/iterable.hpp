#ifndef APEX_CHECK_ITERABLE_HPP
#define APEX_CHECK_ITERABLE_HPP

#include <apex/core/traits.hpp>

namespace apex::check {

template <class I>
concept dereference = requires (remove_cvref_t<I> const& i) { i.dereference(); };

// input
template <class I>
concept equals = requires (remove_cvref_t<I> const& i) {
  { i.equals(i); } -> boolean;
}

// optional input when sentinel_type is 'different_from<self_type>
template <class I, class S>
concept stop = requires (remove_cvref_t<I> const& i, remove_cvref_t<S> const& s) {
  { i.stop(s) } -> boolean;
};

// input
template <class I>
concept increment = requires (remove_cvref_t<I>& i) {
  { i.increment() } -> same_as<void>;
};

// bidirectional
template <class I>
concept decrement = requires (remove_cvref_t<I>& i) {
  { i.decrement() } -> same_as<void>;
};

// random_access
template <class I, class D>
concept advance = requires (remove_cvref_t<I>& i, remove_cvref_t<D> d) {
  { i.advance(d) } -> same_as<void>;
}

// random_access
template <class I, class S>
concept distance_to = requires (remove_cvref_t<I> const& i, remove_cvref_t<S> const& s) {
  i.distance_to(s);
};

} /* namespace apex::check */

#endif /* APEX_CHECK_ITERABLE_HPP */
