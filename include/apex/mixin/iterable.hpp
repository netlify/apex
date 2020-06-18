#ifndef APEX_MIXIN_ITERABLE_HPP
#define APEX_MIXIN_ITERABLE_HPP

// This is a multi-tiered customizable iterable mixin. That is, by making a
// type 'iterable', it can then be turned into a range with little effort.
// There are two ways to plugin an iterable type
// The primary way is to simply implement the 'object' interface. i.e.,
// using .dereference(), .advance(), etc.
// The other is to specialize an iterable_traits type. This makes life easier
// for "fixing" poor iterator interfaces when slowly constructing a wrapper
// around C APIs or similar.

#include <apex/core/concepts.hpp>

namespace apex::mixin {

template <class> struct iterable_traits;
template <class> struct iterable;

template <class I>
concept input_iterable = requires {
  requires check::dereference<I>;
  requires check::increment<I>;
  requires check::equals<I>;
};

template <class I>
concept forward_iterable = requires {
  requires copy_constructible<I>;
  requires input_iterable<I>;
};

template <class I>
concept bidirectional_iterable = requires {
  requires forward_iterable<I>;
  requires check::decrement<I>;
};

template <class I>
concept random_access_iterable = requires {
  requires check::dereference<I>;
  //requires check::distance_to<I>;
  //requires check::advance<I>
};

template <class Derived>
struct iterable {
  using iterable_type = iterable;
  using self_type = Derived;

  decltype(auto) operator * () const noexcept { return this->self().dereference(); }

  auto operator -> () const noexcept(noexcept(std::addressof(**this)))
    requires is_lvalue_reference_v<decltype(**std::declval<iterable_type const&>())>
  { return std::addressof(**this); }

  // noexcept is nothrow copy constructible
  auto operator -> () const& noexcept
    requires requires (iterable_type const& self) {
      requires not is_reference_v<decltype(**self); 
    }
  { return proxy::arrow(**this); }

  // noexcept is nothrow move constructible
  auto operator -> () && noexcept
    requires requires (iterable_type&& self) {
      requires not is_reference_v<decltype(**self)>; 
    }
  { return proxy::arrow(std::move(**this)); }

  self_type& operator ++ ()
    noexcept(noexcept(this->self().increment()))
    requires check::increment<self_type>
  {
    this->self().increment();
    return this->self();
  }

  self_type& operator ++ ()
    noexcept(noexcept(*this += 1))
    requires detect::iterable::advance<self_type>
      and not detect::iterable::increment<self_type>
  {
    this->self().advance(1);
    return this->self();
  }

  self_type operator ++ (int)
    noexcept(is_nothrow_copy_constructible_v<self_type>)
    requires copy_constructible<self_type>
  {
    auto that = this->self();
    ++*this;
    return that;
  }

  self_type operator -- (int)
    noexcept(is_nothrow_copy_constructible_v<self_type>)
    requires copy_constructible<self_type>
  {
    auto that = this->self();
    --*this;
    return that;
  }

  friend bool operator == (self_type const& lhs, self_type const& rhs)
    noexcept(noexcept(lhs.equal_to(rhs)))
    requires (not detect::iterable::distance_to<self_type, self_type>)
  { return lhs.equal_to(rhs); }

private:
  decltype(auto) self () const noexcept { return *static_cast<self_type const*>(*this); }
  decltype(auto) self () noexcept { return *static_cast<self_type*>(*this); }
};

} /* namespace apex::mixin */

namespace std {

template <class I> requires apex::derived_from<I, iterable<I>>
struct iterator_traits<I> {
  using reference = decltype(*declval<remove_cvref_t<I> const&>());
  using pointer = decltype(*declval<remove_cvref_t<I> const&>().operator->());
  //using difference_type = apex::infer_difference_type_t<I>;
  //using value_type = infer_value_type_t<I>;

  using iterator_category = conditional_t<
    random_access_iterable<I>,
    random_access_iterator_tag,
    conditional_t<
      bidirectional_iterable<I>,
      bidirectional_iterator_tag,
      conditional_t<
        forward_iterable<I>,
        forward_iterator_tag,
        conditional_t<
          input_iterable,
          input_iterator_tag,
          apex::empty
        >
      >
    >
  >;
  using iterator_concept = iterator_category;
};
} /* namespace std */

#endif /* APEX_MIXIN_ITERABLE_HPP */
