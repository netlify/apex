#ifndef APEX_CORE_ITERATOR_HPP
#define APEX_CORE_ITERATOR_HPP



#include <apex/detect/types.hpp>
#include <apex/check/alias.hpp>

#include <apex/core/concepts.hpp>
#include <apex/core/prelude.hpp>
#include <iterator>

// TODO: most of these declarations need to be put behind a std shim

namespace apex::detail {

template <class> struct cond_value_type { };
template <class T> requires is_object_v<T>
struct cond_value_type<T> : remove_cv<T> { };

} /* namespace apex::detail */

namespace apex {

template <dereferenceable T>
using iter_reference_t = decltype(*std::declval<T&>());

template <class> struct indirectly_readable_traits { };
template <class T>
struct indirectly_readable_traits<T*> : detail::cond_value_type<T> { };

template <class T> requires is_array_v<T>
struct indirectly_readable_traits<T> : remove_cv<remove_extent_t<T>> { };

template <class T>
struct indirectly_readable_traits<T const> : indirectly_readable_traits<T> { };

template <apex::check::value_type T> requires (not apex::check::element_type<T>)
struct indirectly_readable_traits<T> :
  detail::cond_value_type<typename T::value_type>
{ };

template <apex::check::element_type T> requires (not apex::check::value_type<T>)
struct indirectly_readable_traits<T> :
  detail::cond_value_type<typename T::element_type>
{ };

template <class T>
requires apex::check::element_type<T> and apex::check::value_type<T>
struct indirectly_readable_traits<T> { };

template <class I>
concept incrementable = regular<I> and weakly_incrementable<I> and requires (I i) {
  { i++ } -> same_as<I>;
};

template <class Out, class T>
concept indirectly_writable = requires(Out&& o, T&& t) {
  *o = std::forward<T>(t);
  *std::forward<Out>(o) = std::forward<T>(t);
  const_cast<iter_reference_t<Out> const&&>(*o) = std::forward<T>(t);
  const_cast<iter_reference_t<Out> const&&>(*std::forward<Out>(o)) = std::forward<T>(t);
};

template <class C> requires requires (C const& c) { c.size(); }
constexpr auto ssize (C const& c) {
  using result_type = common_type_t<make_signed_t<decltype(c.size())>, ptrdiff_t>;
  return static_cast<result_type>(c.size());
}

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
