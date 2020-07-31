#ifndef APEX_DETECT_TYPES_HPP
#define APEX_DETECT_TYPES_HPP

namespace apex::detect::types {

template <class T> using difference_type = typename T::difference_type;
template <class T> using element_type = typename T::element_type;
template <class T> using value_type = typename T::value_type;
template <class T> using size_type = typename T::size_type;

template <class T> using reference = typename T::reference;
template <class T> using pointer = typename T::pointer;

} /* namespace apex::detect::types */

#endif /* APEX_DETECT_TYPES_HPP */
