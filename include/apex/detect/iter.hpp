#ifndef APEX_DETECT_ITER_HPP
#define APEX_DETECT_ITER_HPP

#include <iterator>
#include <utility>

namespace apex::detect::iter::adl {

using ::std::declval;

using ::std::begin;
using ::std::end;

using ::std::data;
using ::std::size;

template <class T> using begin_type = decltype(begin(declval<T&>()));
template <class T> using end_type = decltype(end(declval<T&>()));

template <class T> using data_type = decltype(data(declval<T&>()));
template <class T> using size_type = decltype(size(declval<T&>()));

} /* namespace apex::detect::iter::adl */

namespace apex::detect::iter {

template <class T> using begin = adl::begin_type<T>;
template <class T> using end = adl::end_type<T>;

template <class T> using data = adl::data_type<T>;
template <class T> using size = adl::size_type<T>;

} /* namespace apex::detect::iter */

#endif /* APEX_DETECT_ITER_HPP */
