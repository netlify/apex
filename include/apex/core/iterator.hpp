#ifndef APEX_CORE_ITERATOR_HPP
#define APEX_CORE_ITERATOR_HPP

#include <apex/detect/types.hpp>
#include <apex/core/concepts.hpp>
#include <apex/core/types.hpp>
#include <iterator>

// TODO: most of these declarations need to be put behind a std shim

namespace apex {

template <class T>
concept referenceable = requires {
  requires is_lvalue_reference_v<T>;
  requires not same_as<T, void>;
};

template <class T>
concept dereferenceable = requires (T& t) { *t; };

template <class> struct incrementable_traits { };
template <class T>
struct incrementable_traits<T const> :
  incrementable_traits<T>
{ };
template <class T> requires (not std::is_object_v<T>)
struct incrementable_traits<T*> { };
template <class T> requires std::is_object_v<T>
struct incrementable_traits<T*> { using difference_type = ptrdiff_t; };
template <class T> requires requires { typename T::difference_type; }
struct incrementable_traits<T> {
  using difference_type = typename T::difference_type;
};

template <class T> requires dereferenceable<T>
using iter_reference_t = decltype(*std::declval<T&>());

// TODO: use the correct definition (though I doubt we'll need it)
template <class T>
using iter_difference_t = detected_or_t<
  detected_or_t<
    empty,
    detect::types::difference_type,
    incrementable_traits<remove_cvref_t<T>>
  >,
  detect::types::difference_type,
  std::iterator_traits<remove_cvref_t<T>>
>;

// XXX: Not a proper implementation :/
template <class I>
concept weakly_incrementable = default_initializable<I>
  and movable<I>
  and requires (I i) {
    requires signed_integral<iter_difference_t<I>>;
    { ++i } -> same_as<I&>;
    i++;
  };

template <class I>
concept incrementable = regular<I> and weakly_incrementable<I> and requires (I i) {
  { i++ } -> same_as<I>;
};

template <class I>
concept input_or_output_iterator = weakly_incrementable<I>
  and requires (I i) {
    { *i } -> referenceable;
  };

template <class Out, class T>
concept indirectly_writable = requires(Out&& o, T&& t) {
  *o = std::forward<T>(t);
  *std::forward<Out>(o) = std::forward<T>(t);
  const_cast<iter_reference_t<Out> const&&>(*o) = std::forward<T>(t);
  const_cast<iter_reference_t<Out> const&&>(*std::forward<Out>(o)) = std::forward<T>(t);
};

template <class S, class I>
concept sentinel_for = semiregular<S>
  and input_or_output_iterator<I>
  and weak_equality_comparable_with<S, I>;

// TODO: place behind a std shim
template <
  class C, 
  class R=std::common_type_t<
    std::make_signed_t<decltype(std::declval<C const&>().size())>,
    std::ptrdiff_t
  >
> constexpr auto ssize (C const& c) { return static_cast<R>(c.size()); }

// These are custom declarations so they stay here.
template <class Container>
struct back_emplacer {
  using iterator_category = std::output_iterator_tag;
  using difference_type = void;
  using value_type = void;
  using reference = void;
  using pointer = void;
  using container_type = Container;

  back_emplacer (back_emplacer const&) = default;
  back_emplacer () = delete;

  explicit back_emplacer (container_type& container) noexcept :
    container(std::addressof(container))
  { }

  template <class T>
  back_emplacer& operator = (T&& t)
    noexcept(noexcept(this->container->emplace_back(std::forward<T>(t))))
    requires requires (container_type* container) { container->emplace_back(std::forward<T>(t)); }
  {
    this->container.emplace_back(std::forward<T>(t));
    return *this;
  }

  back_emplacer& operator * () { return *this; }
  back_emplacer& operator ++ () { return *this; }
  back_emplacer& operator ++ (int) { return *this; }

protected:
  container_type* container;
};

template <class Container> back_emplacer (Container&) -> back_emplacer<Container>;

} /* namespace apex */

#endif /* APEX_CORE_ITERATOR_HPP */
