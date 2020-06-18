#ifndef APEX_CHECK_ALIAS_HPP
#define APEX_CHECK_ALIAS_HPP

namespace apex::check {

template <class T> concept difference_type = requires { typename T::difference_type; };
template <class T> concept element_type = requires { typename T::element_type; };
template <class T> concept value_type = requires { typename T::value_type; };
template <class T> concept size_type = requires { typename T::size_type; };
template <class T> concept reference = requires { typename T::reference; };
template <class T> concept pointer = requires { typename T::pointer; };

} /* namespace apex::check */

#endif /* APEX_CHECK_ALIAS_HPP */
