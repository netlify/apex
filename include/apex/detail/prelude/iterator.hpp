#ifndef APEX_DETAIL_PRELUDE_ITERATOR_HPP
#define APEX_DETAIL_PRELUDE_ITERATOR_HPP

#include <apex/detail/prelude/detected.hpp>
#include <apex/detail/prelude/concepts.hpp>
#include <apex/detail/prelude/types.hpp>

#include <iterator>

#if not APEX_CHECK_API(ranges, 201911)
namespace apex::detail::prelude {

template <class T> using difference_type = typename T::difference_type;

template <class T>
concept difference = requires { typename T::difference_type; };

template <class T>
concept subtractable = not difference<T>
  and requires (T const& x, T const& y) {
    { x - y } -> integral;
  };

} /* namespace apex::detail::prelude */
#endif /* not APEX_CHECK_API(ranges, 201911) */

namespace apex {

#if APEX_CHECK_API(ranges, 201911)
  APEX_WARN("std::ranges is available. A refactor is needed")
  using ::std::input_or_output_iterator;
  using ::std::weakly_incrementable;
  using ::std::sentinel_for;
  using ::std::incrementable_traits;
  using ::std::iter_difference_t;
#else

template <class> struct incrementable_traits { };

template <class T> requires ::std::is_object_v<T>
struct incrementable_traits<T*> { using difference_type = ptrdiff_t; };

template <class I>
struct incrementable_traits<I const> :
  incrementable_traits<I>
{ };

template <detail::prelude::difference T>
struct incrementable_traits<T> {
  using difference_type = typename T::difference_type;
};

template <detail::prelude::subtractable T>
struct incrementable_traits<T> {
  using difference_type = ::std::make_signed_t<
    decltype(::std::declval<T>() - ::std::declval<T>())
  >;
};

template <class T>
using iter_difference_t = detected_or_t<
  detected_t<detail::prelude::difference_type, incrementable_traits<remove_cvref_t<T>>>,
  detail::prelude::difference_type,
  ::std::iterator_traits<remove_cvref_t<T>>
>;

/* This is technically incorrect because we do not use 'is-signed-integer-like,
 * but instead use signed_integral. That said we don't really *need* that, as
 * we aren't going to need anything beyond what `signed_integral` provides
 * If we use numeric_limits<T> we can find if something is an integer-class-type
 * or signed-integer-class-type, which is needed for is-signed-integer-like
 * and is-integer-like from the definition.
 * The full description can be found starting at [iterator.concept.winc]/6
 */
template <class I>
concept weakly_incrementable = default_initializable<I>
  and movable<I>
  and requires (I i) {
    typename iter_difference_t<I>;
    requires signed_integral<iter_difference_t<I>>;
    { ++i } -> same_as<I&>;
    i++;
  };

template <class I>
concept input_or_output_iterator = weakly_incrementable<I>
  and requires (I i) {
    { *i } -> different_from<void>;
  };

template <class S, class I>
concept sentinel_for = semiregular<S>
  and input_or_output_iterator<I>
  and weakly_equality_comparable_with<S, I>;

#endif /* APEX_CHECK_API(ranges, 201911) */

} /* namespace apex */

#endif /* APEX_DETAIL_PRELUDE_ITERATOR_HPP */
