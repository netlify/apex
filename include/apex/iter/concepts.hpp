#ifndef APEX_ITER_CONCEPTS_HPP
#define APEX_ITER_CONCEPTS_HPP

#include <apex/iter/distance-to.hpp>
#include <apex/iter/write-into.hpp>
#include <apex/iter/read-from.hpp>
#include <apex/iter/equal-to.hpp>
#include <apex/iter/advance.hpp>
#include <apex/iter/next.hpp>
#include <apex/iter/prev.hpp>

#include <apex/iter/traits.hpp>

#include <apex/core/concepts.hpp>

/* Our concepts are similarly named to the ones found in the standard but 
 * ONLY apply to a specific expression found with our CPOs for implementing
 * iterable mixins. The rules are more 'lax' than the standard counterparts
 * because these interfaces are used to implement an iterator that would *then*
 * properly meet the standard concepts.
 */
namespace apex::iter {

/** @interface apex::iter::indirectly_readable_from<I> apex/iter/concepts.hpp
 * @concept{iterable}
 * @brief Specifies that @ref apex::iter::read_from can be used on a type.
 *
 * This is primarily used to implement the input_iterator capabilities on @ref
 * apex::mixin::iterator.
 * @see apex::iter::read_from
 */
template <class I>
concept indirectly_readable_from = requires (I iterable) {
  { ::apex::iter::read_from(iterable) } -> distinct_from<void>;
};

/** @interface apex::iter::indirectly_writable_into<I, T> apex/iter/concepts.hpp
 * @concept{iterable} 
 * @brief Specifies that @ref apex::iter::write_into can be used on a type.
 *
 * This is primarily used to implement the output_iterator capabilities on the
 * @ref apex::mixin::iterator
 * @see @link .write_into() apex::iter::write_into @endlink
 * @note The underlying type of `T` must differ from the underlying type of `I`.
 * This is enforced with @ref apex::different_from
 */
template <class I, class T>
concept indirectly_writable_into = different_from<I, T>
  and requires (I& iterable, T&& value) {
    { ::apex::iter::write_into(iterable, static_cast<T&&>(value)) } -> same_as<void>;
  };

/** @interface apex::iter::indirectly_addressable<I> apex/iter/concepts.hpp
 * @concept{iterable} 
 * @brief Specifies that the result of @ref apex::iter::read_from can be used
 * with `std::addressof`.
 * @details This is primarily used to detect if @ref apex::proxy::arrow should
 * be used.
 */
template <class I>
concept indirectly_addressable = indirectly_readable_from<I>
  and requires (I iterable) {
    requires ::std::is_lvalue_reference_v<decltype(::apex::iter::read_from(iterable))>;
  };

/** @interface weakly_incrementable<I> apex/iter/concepts.hpp
 * @details
 * @concept{iterable} Meant to return the next successor of @c I, this
 * relies entirely on ::#next. 
 * @code{.cpp}
 * requires (I iterable) {
 *   { ::apex::iter::next(iterable) } -> same_as<void>;
 * }
 * @endcode
 */
template <class I>
concept weakly_incrementable = requires (I iterable) {
  { ::apex::iter::next(iterable) } -> same_as<void>;
};

template <class I>
concept weakly_decrementable = requires (I iterable) {
  { ::apex::iter::prev(iterable) } -> same_as<void>;
};

template <class I, class D=iter_difference_t<I>>
concept weakly_steppable = requires (I iterable, D offset) {
  { ::apex::iter::advance(iterable, offset) } -> same_as<void>;
};

template <class I, class S=sentinel_of_t<I>>
concept distance_measurable = requires (I iterable, S sentinel) {
  { ::apex::iter::distance_to(iterable, sentinel) } -> signed_integral;
};

template <class I, class S=sentinel_of_t<I>>
concept equality_comparable = requires (I iterable, S sentinel) {
  { ::apex::iter::equal_to(iterable, sentinel) } -> convertible_to<bool>;
};

template <class I>
concept input_iterator = weakly_incrementable<I>
  and indirectly_readable_from<I>
  and equality_comparable<I>;

template <class I>
concept forward_iterator = input_iterator<I>
  and copy_constructible<I>;

template <class I>
concept bidirectional_iterator = forward_iterator<I>
  and weakly_decrementable<I>;

template <class I>
concept random_access_iterator = bidirectional_iterator<I>
  and weakly_steppable<I, iter_difference_t<I>>
  and distance_measurable<I>;

template <class I, class T>
concept output_iterator = weakly_incrementable<I>
  and indirectly_writable_into<I, T>;

} /* namespace apex::iter */

#endif /* APEX_ITER_CONCEPTS_HPP */
