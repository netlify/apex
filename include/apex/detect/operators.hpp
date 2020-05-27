#ifndef APEX_DETECT_OPERATORS_HPP
#define APEX_DETECT_OPERATORS_HPP

#include <utility>

namespace apex::detect {

using ::std::declval;

} /* apex::detect */

namespace apex::detect::prefix {

template <class T> using negate = decltype(-declval<T>());
template <class T> using postincrement = decltype(declval<T>()++);
template <class T> using postdecrement = decltype(declval<T>()--);
template <class T> using increment = decltype(++declval<T>());
template <class T> using decrement = decltype(--declval<T>());

} /* namespace apex::detect::prefix */

namespace apex::detect::comparison {

template <class T, class U> using not_equal_to = decltype(declval<T>() != declval<U>());
template <class T, class U> using equal_to = decltype(declval<T>() == declval<U>());

template <class T, class U> using greater_equal = decltype(declval<T>() >= declval<U>());
template <class T, class U> using less_equal = decltype(declval<T>() <= declval<U>());
template <class T, class U> using greater = decltype(declval<T>() > declval<U>());
template <class T, class U> using less = decltype(declval<T>() < declval<U>());

} /* namespace apex::detect::comparison */

namespace apex::detect::arithmetic {

template <class T, class U> using multiplies = decltype(declval<T>() * declval<U>());
template <class T, class U> using divides = decltype(declval<T>() / declval<U>());
template <class T, class U> using modulus = decltype(declval<T>() % declval<U>());
template <class T, class U> using minus = decltype(declval<T>() - declval<U>());
template <class T, class U> using plus = decltype(declval<T>() + declval<U>());

} /* namespace apex::detect::arithmetic */

namespace apex::detect::bitwise {

template <class T, class U> using bit_and = decltype(declval<T>() & declval<U>());
template <class T, class U> using bit_xor = decltype(declval<T>() ^ declval<U>());
template <class T, class U> using bit_or = decltype(declval<T>() | declval<U>());

template <class T, class U> using bit_not = decltype(~declval<T>());

} /* namespace apex::detect::bitwise */

namespace apex::detect::pointer {

template <class T, class I> using subscript = decltype(declval<T>()[declval<I>()]);
template <class T> using dereference = decltype(*declval<T>());
template <class T> using arrow = decltype(declval<T>().operator->());

} /* namespace apex::detect::object */

#endif /* APEX_DETECT_OPERATORS_HPP */
