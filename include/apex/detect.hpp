#ifndef APEX_DETECT_HPP
#define APEX_DETECT_HPP

#include <utility>

namespace apex {
inline namespace v1 {

using ::std::declval;

// Detection meta-functions
/* These will be obsolete under C++20. However, we're still targeting C++17 for
 * the forseeable future.
 */
namespace detect::op {
/* prefix */
template <class T> using negate = decltype(-declval<T>());
template <class T> using postincrement = decltype(declval<T>()++);
template <class T> using postdecrement = decltype(declval<T>()--);
template <class T> using increment = decltype(++declval<T>());
template <class T> using decrement = decltype(--declval<T>());

/* Comparison */
template <class T, class U> using ne = decltype(declval<T>() != declval<U>());
template <class T, class U> using eq = decltype(declval<T>() == declval<U>());
template <class T, class U> using ge = decltype(declval<T>() >= declval<U>());
template <class T, class U> using le = decltype(declval<T>() <= declval<U>());
template <class T, class U> using gt = decltype(declval<T>() > declval<U>());
template <class T, class U> using lt = decltype(declval<T>() < declval<U>());

/* Arithmetic */
template <class T, class U> using mul = decltype(declval<T>() * declval<U>());
template <class T, class U> using div = decltype(declval<T>() / declval<U>());
template <class T, class U> using mod = decltype(declval<T>() % declval<U>());
template <class T, class U> using sub = decltype(declval<T>() - declval<U>());
template <class T, class U> using add = decltype(declval<T>() + declval<U>());

/* Bitwise */
template <class T, class U> using band = decltype(declval<T>() & declval<U>());
template <class T, class U> using bxor = decltype(declval<T>() ^ declval<U>());
template <class T, class U> using bor = decltype(declval<T>() | declval<U>());
template <class T, class U> using bsl = decltype(declval<T>() << declval<U>());
template <class T, class U> using bsr = decltype(declval<T>() >> declval<U>());
template <class T> using bnot = decltype(~declval<T>());

/* Arithmethic assign */
template <class T, class U> using imul = decltype(declval<T>() *= declval<U>());
template <class T, class U> using idiv = decltype(declval<T>() /= declval<U>());
template <class T, class U> using imod = decltype(declval<T>() %= declval<U>());
template <class T, class U> using iadd = decltype(declval<T>() += declval<U>());
template <class T, class U> using isub = decltype(declval<T>() -= declval<U>());

/* Bitwise assign */
template <class T, class U> using iand = decltype(declval<T>() &= declval<U>());
template <class T, class U> using ixor = decltype(declval<T>() ^= declval<U>());
template <class T, class U> using ior = decltype(declval<T>() |= declval<U>());
template <class T, class U> using isl = decltype(declval<T>() <<= declval<U>());
template <class T, class U> using isr = decltype(declval<T>() >>= declval<U>());

/* members */
template <class T, class I> using subscript = decltype(declval<T>()[declval<I>()]);
template <class T> using dereference = decltype(*declval<T>());
template <class T> using arrow = decltype(declval<T>().operator->());
} /* namespace detect::op */


namespace detect {

template <class T> using difference_type = typename T::difference_type;
template <class T> using element_type = typename T::element_type;
template <class T> using value_type = typename T::value_type;
template <class T> using size_type = typename T::size_type;

template <class T> using reference = typename T::reference;
template <class T> using pointer = typename T::pointer;

template <class T> using complete = decltype(void(sizeof(T)));

} /* namespace detect */

namespace detect {
namespace impl {

} /* namespace impl */


} /* namespace detect */

}} /* apex::v1 */

#endif /* APEX_DETECT_HPP */
