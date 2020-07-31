#ifndef APEX_CORE_META_HPP
#define APEX_CORE_META_HPP

/** These are intended to be used with the apex::detectable_with concept.
 * Normally we place these items into just the top-level namespace.  However,
 * this ends up causing some naming conflicts and potential shadowing issues so
 * these are currently placed into a apex::detect:: namespace to fix the issue.
 * @todo The organization of these meta functions, as well as their interaction
 * with apex::detectable_with are... not great. A better system *really is*
 * needed, or we just need to declare them as implementation details only
 * in a per-header/per-module partition sort of way.
 */
namespace apex::meta {

template <class T> using difference_type = typename T::difference_type;
template <class T> using element_type = typename T::element_type;
template <class T> using value_type = typename T::value_type;
template <class T> using size_type = typename T::size_type;
template <class T> using reference = typename T::reference;
template <class T> using pointer = typename T::pointer;

} /* namespace apex::meta */

#endif /* APEX_CORE_META_HPP */
